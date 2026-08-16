// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GCameraComponent.h"
#include "Camera/CameraTypes.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "Guest/Components/GPhotoSubjectComponent.h"
#include "Guest/Subsystem/GPhotoSubjectRegistrySubsystem.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "Guest/Items/Instance/GPhotoItemInstanceData.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/UI/Subsystems/GPhotoLibrarySubsystem.h"
#include "Guest/Utils/GLog.h"

namespace
{
	// 카메라에서 쏜 광선이 박스에 처음 닿는 거리(근접면까지)를 구한다. 슬랩 방식.
	bool ComputeBoxEntryDistance(const FVector& Origin, const FVector& Dir,
		const FVector& BoxCenter, const FVector& BoxExtent, float& OutDistance)
	{
		const FVector BoxMin = BoxCenter - BoxExtent;
		const FVector BoxMax = BoxCenter + BoxExtent;

		double TMin = 0.0;
		double TMax = TNumericLimits<double>::Max();

		for (int32 Axis = 0; Axis < 3; ++Axis)
		{
			const double D = Dir[Axis];
			const double O = Origin[Axis];

			if (FMath::IsNearlyZero(D))
			{
				// 이 축으로는 평행하게 지나간다 — 애초에 밖이면 영영 안 만난다
				if (O < BoxMin[Axis] || O > BoxMax[Axis])
				{
					return false;
				}
				continue;
			}

			double T1 = (BoxMin[Axis] - O) / D;
			double T2 = (BoxMax[Axis] - O) / D;
			if (T1 > T2)
			{
				Swap(T1, T2);
			}

			TMin = FMath::Max(TMin, T1);
			TMax = FMath::Min(TMax, T2);
			if (TMin > TMax)
			{
				return false;
			}
		}

		OutDistance = static_cast<float>(TMin);
		return true;
	}

	// 박스의 중심·8꼭짓점 **방향**으로 광선을 쏘되, 표본은 박스에 들어가기 직전까지만 본다.
	//
	// 꼭짓점 자체를 표적으로 삼으면 지형에 파묻힌 아래쪽 네 점이 늘 가려진 것으로 잡혀,
	// 눈에 뻔히 보이는 건물이 "안 보인다"고 판정된다. 근접면까지만 재면 피사체 자신의
	// 지오메트리가 가림으로 잡히는 일도 없어서, 첫 충돌이 박스 안인지 따질 필요가 사라진다.
	int32 CountVisibleSamples(const UWorld* World, const FVector& CamLoc,
		const FVector& BoxCenter, const FVector& BoxExtent, const AActor* IgnoreActor)
	{
		if (!World)
		{
			return 0;
		}

		FCollisionQueryParams Params(SCENE_QUERY_STAT(GPhotoSubjectVisibility), /*bTraceComplex=*/false);
		if (IgnoreActor)
		{
			Params.AddIgnoredActor(IgnoreActor);
		}

		int32 VisibleCount = 0;
		for (int32 Index = 0; Index < 9; ++Index)
		{
			FVector Target = BoxCenter;
			if (Index < 8)
			{
				Target += FVector(
					(Index & 1) ? BoxExtent.X : -BoxExtent.X,
					(Index & 2) ? BoxExtent.Y : -BoxExtent.Y,
					(Index & 4) ? BoxExtent.Z : -BoxExtent.Z);
			}

			const FVector Dir = (Target - CamLoc).GetSafeNormal();
			if (Dir.IsNearlyZero())
			{
				continue;
			}

			float EntryDistance = 0.0f;
			if (!ComputeBoxEntryDistance(CamLoc, Dir, BoxCenter, BoxExtent, EntryDistance))
			{
				continue;
			}

			// 근접면 바로 앞에서 멈춘다
			const float StopDistance = EntryDistance - 2.0f;
			if (StopDistance <= 1.0f)
			{
				// 박스가 코앞이라 가릴 것이 낄 자리가 없다
				++VisibleCount;
				continue;
			}

			if (!World->LineTraceTestByChannel(CamLoc, CamLoc + Dir * StopDistance, ECC_Visibility, Params))
			{
				++VisibleCount;
			}
		}

		return VisibleCount;
	}
}

UGCameraComponent::UGCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

float UGCameraComponent::ScoreSubject(const UGPhotoSubjectComponent* Subject, float& OutCoverage) const
{
	OutCoverage = 0.0f;

	if (!Subject || !CaptureComponent || !RenderTarget)
	{
		return 0.0f;
	}

	const int32 Width  = RenderTarget->SizeX;
	const int32 Height = RenderTarget->SizeY;
	if (Width <= 0 || Height <= 0)
	{
		return 0.0f;
	}

	// 직교 투영에는 아래의 원근 나눗셈이 성립하지 않는다. 디지캠은 원근이므로 여기 오지 않는다.
	if (CaptureComponent->ProjectionType != ECameraProjectionMode::Perspective)
	{
		return 0.0f;
	}

	FVector BoxCenter, BoxExtent;
	Subject->GetWorldBox(BoxCenter, BoxExtent);

	const FTransform CamXform = CaptureComponent->GetComponentTransform();
	const FVector CamLoc = CamXform.GetLocation();

	// 카메라가 피사체 박스 안에 있으면 후보에서 뺀다.
	// 그 안에 서 있는 것과 그것을 찍는 것은 다르다. 빼지 않으면 박스가 어느 방향을 보든
	// 화면을 뒤덮어 점유율이 100%로 잡히고, 하늘을 찍어도 그 피사체가 1등이 된다 —
	// 좌표 판정에서 고치려던 문제가 그대로 되살아난다.
	const FVector CamToCenter = (CamLoc - BoxCenter).GetAbs();
	if (CamToCenter.X <= BoxExtent.X && CamToCenter.Y <= BoxExtent.Y && CamToCenter.Z <= BoxExtent.Z)
	{
		return 0.0f;
	}

	// 회전행렬의 전치가 곧 역행렬이다 (직교행렬). 월드 → 카메라 로컬(X 정면, Y 오른쪽, Z 위).
	const FMatrix WorldToCam = FRotationMatrix(CamXform.Rotator()).GetTransposed();

	const float Aspect = static_cast<float>(Width) / static_cast<float>(Height);
	const float HTan = FMath::Tan(FMath::DegreesToRadians(CaptureComponent->FOVAngle * 0.5f));
	if (HTan <= KINDA_SMALL_NUMBER || Aspect <= KINDA_SMALL_NUMBER)
	{
		return 0.0f;
	}
	const float VTan = HTan / Aspect;

	// 8꼭짓점을 NDC로 투영해 화면상 사각형을 구한다.
	// 카메라 뒤로 넘어간 꼭짓점은 버린다 — 피사체 안에 들어가 있는 경우는 판정하지 않는다.
	FVector2D RectMin( TNumericLimits<float>::Max(),  TNumericLimits<float>::Max());
	FVector2D RectMax(-TNumericLimits<float>::Max(), -TNumericLimits<float>::Max());
	int32 InFrontCount = 0;

	for (int32 Index = 0; Index < 8; ++Index)
	{
		const FVector Corner = BoxCenter + FVector(
			(Index & 1) ? BoxExtent.X : -BoxExtent.X,
			(Index & 2) ? BoxExtent.Y : -BoxExtent.Y,
			(Index & 4) ? BoxExtent.Z : -BoxExtent.Z);

		const FVector Local = WorldToCam.TransformVector(Corner - CamLoc);
		if (Local.X <= 1.0f)
		{
			continue;
		}
		++InFrontCount;

		const float NdcX = static_cast<float>(Local.Y / Local.X) / HTan;
		const float NdcY = static_cast<float>(Local.Z / Local.X) / VTan;

		RectMin.X = FMath::Min(RectMin.X, NdcX);
		RectMin.Y = FMath::Min(RectMin.Y, NdcY);
		RectMax.X = FMath::Max(RectMax.X, NdcX);
		RectMax.Y = FMath::Max(RectMax.Y, NdcY);
	}

	if (InFrontCount == 0)
	{
		return 0.0f;
	}

	// 화면 밖으로 나간 부분은 사진에 남지 않으므로 잘라낸다
	const FVector2D ClippedMin(FMath::Max(RectMin.X, -1.0f), FMath::Max(RectMin.Y, -1.0f));
	const FVector2D ClippedMax(FMath::Min(RectMax.X,  1.0f), FMath::Min(RectMax.Y,  1.0f));
	if (ClippedMin.X >= ClippedMax.X || ClippedMin.Y >= ClippedMax.Y)
	{
		return 0.0f;
	}

	// NDC 전체 넓이가 2×2이므로 4로 나누면 화면 점유율이 된다
	const float Coverage = static_cast<float>((ClippedMax.X - ClippedMin.X) * (ClippedMax.Y - ClippedMin.Y)) * 0.25f;
	OutCoverage = Coverage;

	if (Coverage < Subject->MinScreenCoverage)
	{
		return 0.0f;
	}

	// 중앙 근접도 — 같은 크기로 담겼다면 가운데 둔 쪽이 "그걸 찍은" 것이다.
	// NDC 원점에서 모서리까지가 √2이므로 그것으로 정규화한다.
	static const float NdcCornerDistance = FMath::Sqrt(2.0f);
	const FVector2D RectCenter = (ClippedMin + ClippedMax) * 0.5f;
	const float Centrality = FMath::Clamp(1.0f - static_cast<float>(RectCenter.Size()) / NdcCornerDistance, 0.0f, 1.0f);

	float VisibleRatio = 1.0f;
	if (bCheckOcclusion)
	{
		const int32 VisibleSamples = CountVisibleSamples(GetWorld(), CamLoc, BoxCenter, BoxExtent, GetOwner());
		if (VisibleSamples == 0)
		{
			return 0.0f;
		}
		VisibleRatio = static_cast<float>(VisibleSamples) / 9.0f;

		if (VisibleRatio < Subject->MinVisibleRatio)
		{
			return 0.0f;
		}
	}

	return Coverage * Centrality * VisibleRatio * Subject->ScoreWeight;
}

FName UGCameraComponent::ResolveSubject() const
{
	if (SubjectResolution == EPhotoSubjectResolution::PlaceOnly)
	{
		return NAME_None;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return NAME_None;
	}

	UGPhotoSubjectRegistrySubsystem* Registry = World->GetSubsystem<UGPhotoSubjectRegistrySubsystem>();
	if (!Registry)
	{
		return NAME_None;
	}

	TArray<UGPhotoSubjectComponent*> Subjects;
	Registry->GetSubjects(Subjects);
	if (Subjects.Num() == 0)
	{
		return NAME_None;
	}

	FName BestID = NAME_None;
	float BestScore = 0.0f;
	float BestCoverage = 0.0f;

	for (const UGPhotoSubjectComponent* Subject : Subjects)
	{
		float Coverage = 0.0f;
		const float Score = ScoreSubject(Subject, Coverage);
		if (Score > BestScore)
		{
			BestScore = Score;
			BestCoverage = Coverage;
			BestID = Subject->SubjectID;
		}
	}

	if (!BestID.IsNone())
	{
		G_LOG(TEXT("피사체 판정: %s (점유율 %.1f%%, 점수 %.4f, 후보 %d)"),
			*BestID.ToString(), BestCoverage * 100.0f, BestScore, Subjects.Num());
	}

	return BestID;
}

FName UGCameraComponent::ResolveSubjectID(FName PlaceSubjectID) const
{
	switch (SubjectResolution)
	{
	case EPhotoSubjectResolution::PlaceOnly:
		return PlaceSubjectID;

	case EPhotoSubjectResolution::FrameOnly:
		return ResolveSubject();

	case EPhotoSubjectResolution::FrameThenPlace:
	default:
		{
			const FName Framed = ResolveSubject();
			return Framed.IsNone() ? PlaceSubjectID : Framed;
		}
	}
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

		// 뷰파인더는 기본적으로 꺼둔다. 켜져 있으면 씬 전체가 매 프레임 두 번 그려지므로
		// 디지캠을 꺼내지도 않은 평상시에 그 비용을 내서는 안 된다.
		CaptureComponent->bCaptureEveryFrame = false;
	}
}

void UGCameraComponent::SetViewfinderActive(bool bActive)
{
	if (!CaptureComponent) return;
	if (CaptureComponent->bCaptureEveryFrame == bActive) return;

	CaptureComponent->bCaptureEveryFrame = bActive;

	// 켜는 순간 한 장 그려두지 않으면 첫 프레임에 이전(또는 빈) 화면이 보인다
	if (bActive)
	{
		CaptureComponent->CaptureScene();
	}
}

bool UGCameraComponent::IsViewfinderActive() const
{
	return CaptureComponent && CaptureComponent->bCaptureEveryFrame;
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

	// bCaptureEveryFrame이 켜져 있으면(뷰파인더 실시간 표시) 렌더타겟은 이미 최신이다.
	// 그 위에 CaptureScene을 또 부르면 같은 장면을 두 번 그리게 되고 엔진이 경고를 띄운다.
	if (!CaptureComponent->bCaptureEveryFrame)
	{
		CaptureComponent->CaptureScene();
	}

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
