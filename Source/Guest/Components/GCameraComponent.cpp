// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "Guest/Items/Instance/GPhotoItemInstanceData.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/UI/Subsystems/GPhotoLibrarySubsystem.h"
#include "Guest/Utils/GLog.h"

UGCameraComponent::UGCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGCameraComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGCameraComponent::SetupCapture(USceneCaptureComponent2D* InCapture, UTextureRenderTarget2D* InRenderTarget)
{
	CaptureComponent = InCapture;
	RenderTarget = InRenderTarget;

	if (CaptureComponent && RenderTarget)
	{
		CaptureComponent->TextureTarget = RenderTarget;
		CaptureComponent->bCaptureEveryFrame = true;
	}
}

bool UGCameraComponent::TakePhoto(const FPhotoData& Metadata)
{
	if (!CaptureComponent || !RenderTarget)
	{
		G_WARN(TEXT("카메라: CaptureComponent 또는 RenderTarget 미설정"));
		return false;
	}

	if (!PhotoItemDefinition)
	{
		G_WARN(TEXT("카메라: PhotoItemDefinition 미설정 — 사진을 아이템으로 만들 수 없습니다."));
		return false;
	}

	UGInventoryComponent* InvComp = GetOwner() ? GetOwner()->FindComponentByClass<UGInventoryComponent>() : nullptr;
	if (!InvComp)
	{
		G_WARN(TEXT("카메라: 오너에 인벤토리 컴포넌트가 없습니다."));
		return false;
	}

	// 픽셀 읽기·PNG 압축은 무거우므로 인벤토리 공간부터 확인하고 시작한다
	FIntPoint PhotoSize(1, 1);
	if (const UGItemFragmentInventory* InvFrag = PhotoItemDefinition->FindFragmentByClass<UGItemFragmentInventory>())
	{
		PhotoSize = InvFrag->GridSize;
	}
	if (!InvComp->HasSpaceForItem(PhotoSize))
	{
		G_WARN(TEXT("카메라: 인벤토리에 공간이 없어 촬영을 취소합니다. (필요 %dx%d)"), PhotoSize.X, PhotoSize.Y);
		return false;
	}

	CaptureComponent->CaptureScene();

	// GPU → CPU 픽셀 읽기 (동기식 — 촬영 시에만 호출되므로 허용)
	TArray<FColor> Pixels;
	FRenderTarget* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource || !RTResource->ReadPixels(Pixels))
	{
		G_WARN(TEXT("카메라: 픽셀 읽기 실패"));
		return false;
	}

	const int32 Width  = RenderTarget->SizeX;
	const int32 Height = RenderTarget->SizeY;

	UGameInstance* GI = GetWorld()->GetGameInstance();

	UTexture2D* Snapshot = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	if (!Snapshot)
	{
		G_WARN(TEXT("카메라: 스냅샷 텍스처 생성 실패"));
		return false;
	}

	// Outer를 GameInstance로 변경 → 레벨 전환 후에도 GC되지 않음
	Snapshot->Rename(nullptr, GI, REN_DontCreateRedirectors | REN_ForceNoResetLoaders);

	// SceneCapture RT는 알파가 0으로 기록됨 → UImage 투명으로 렌더링되므로 강제 불투명 처리
	for (FColor& Pixel : Pixels)
	{
		Pixel.A = 255;
	}

	void* TexData = Snapshot->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TexData, Pixels.GetData(), Pixels.Num() * sizeof(FColor));
	Snapshot->GetPlatformData()->Mips[0].BulkData.Unlock();
	Snapshot->UpdateResource();

	FPhotoData NewPhoto    = Metadata;
	NewPhoto.Snapshot      = Snapshot;
	NewPhoto.RealWorldTime = FDateTime::Now();

	// 세이브 파일에 사진을 남기기 위해 촬영 시점에 PNG로 압축해 보관 (텍스처는 직렬화 불가)
	TArray64<uint8> PngData;
	FImageUtils::PNGCompressImageArray(Width, Height,
		TArrayView64<const FColor>(Pixels.GetData(), Pixels.Num()), PngData);
	NewPhoto.CompressedImage.Reset();
	NewPhoto.CompressedImage.Append(PngData.GetData(), static_cast<int32>(PngData.Num()));

	// 사진의 저장소는 인벤토리 하나뿐이다 — 갤러리는 이걸 읽어 보여주는 뷰일 뿐
	FGPhotoItemInstanceData PhotoInstanceData;
	PhotoInstanceData.PhotoData = NewPhoto;

	const FInventoryItemHandle Handle =
		InvComp->GrantItemWithData(PhotoItemDefinition, FInstancedStruct::Make(PhotoInstanceData));

	if (!Handle.IsValid())
	{
		// 위에서 공간을 확인했으므로 정상적으로는 도달하지 않는다
		G_WARN(TEXT("카메라: 사진 아이템 지급 실패"));
		return false;
	}

	if (UGameInstance* PhotoGI = GetWorld()->GetGameInstance())
	{
		// 갤러리 실시간 갱신용 — 인벤토리를 다시 훑지 않고 방금 찍은 것만 밀어넣는다
		if (UGPhotoLibrarySubsystem* PhotoLib = PhotoGI->GetSubsystem<UGPhotoLibrarySubsystem>())
		{
			PhotoLib->NotifyPhotoTaken(NewPhoto);
		}

		// 촬영 대상이 지정된 좌표에서만 사진 퀘스트 목표가 진행된다
		if (!NewPhoto.SubjectID.IsNone())
		{
			if (UGQuestSubsystem* QuestSys = PhotoGI->GetSubsystem<UGQuestSubsystem>())
			{
				QuestSys->OnObjectiveUpdated.Broadcast(NewPhoto.SubjectID, 1);
			}
		}
	}

	OnPhotoTaken.Broadcast(NewPhoto);
	G_LOG(TEXT("사진 촬영 완료: %d년 %s"), NewPhoto.InGameYear, *NewPhoto.PlaceName.ToString());
	return true;
}
