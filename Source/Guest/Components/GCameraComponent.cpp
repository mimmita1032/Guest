// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
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

void UGCameraComponent::TakePhoto(const FPhotoData& Metadata)
{
	if (!CaptureComponent || !RenderTarget)
	{
		G_WARN(TEXT("카메라: CaptureComponent 또는 RenderTarget 미설정"));
		return;
	}

	CaptureComponent->CaptureScene();

	// GPU → CPU 픽셀 읽기 (동기식 — 촬영 시에만 호출되므로 허용)
	TArray<FColor> Pixels;
	FRenderTarget* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource || !RTResource->ReadPixels(Pixels))
	{
		G_WARN(TEXT("카메라: 픽셀 읽기 실패"));
		return;
	}

	const int32 Width  = RenderTarget->SizeX;
	const int32 Height = RenderTarget->SizeY;

	UGameInstance* GI = GetWorld()->GetGameInstance();

	UTexture2D* Snapshot = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	if (!Snapshot)
	{
		G_WARN(TEXT("카메라: 스냅샷 텍스처 생성 실패"));
		return;
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

	UGameInstance* PhotoGI = GetWorld()->GetGameInstance();
	if (UGPhotoLibrarySubsystem* PhotoLib = PhotoGI->GetSubsystem<UGPhotoLibrarySubsystem>())
	{
		PhotoLib->AddPhoto(NewPhoto);
	}

	OnPhotoTaken.Broadcast(NewPhoto);
	G_LOG(TEXT("사진 촬영 완료: %d년 %s"), NewPhoto.InGameYear, *NewPhoto.PlaceName.ToString());
}
