// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
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

	UGameInstance* GI = GetWorld()->GetGameInstance();

	// GameInstance를 outer로 지정해야 레벨 전환 후에도 RT가 GC되지 않음
	UTextureRenderTarget2D* PhotoRT = NewObject<UTextureRenderTarget2D>(GI);
	PhotoRT->RenderTargetFormat = RenderTarget->RenderTargetFormat;
	PhotoRT->InitAutoFormat(RenderTarget->SizeX, RenderTarget->SizeY);
	PhotoRT->bAutoGenerateMips = false;
	PhotoRT->UpdateResource();

	FPhotoData NewPhoto = Metadata;
	NewPhoto.RenderTarget = PhotoRT;
	NewPhoto.RealWorldTime = FDateTime::Now();

	// 서브시스템에 저장 — 레벨 전환 후에도 유지됨
	if (UGPhotoLibrarySubsystem* PhotoLib = GI->GetSubsystem<UGPhotoLibrarySubsystem>())
	{
		PhotoLib->AddPhoto(NewPhoto);
	}

	OnPhotoTaken.Broadcast(NewPhoto);
	G_LOG(TEXT("사진 촬영 완료: %d년 %s"), NewPhoto.InGameYear, *NewPhoto.PlaceName.ToString());
}
