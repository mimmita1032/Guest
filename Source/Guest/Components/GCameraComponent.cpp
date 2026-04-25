// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
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
	}
}

void UGCameraComponent::TakePhoto()
{
	if (!CaptureComponent || !RenderTarget)
	{
		G_WARN(TEXT("카메라: CaptureComponent 또는 RenderTarget 미설정"));
		return;
	}

	CaptureComponent->CaptureScene();

	UTextureRenderTarget2D* PhotoRT = UKismetRenderingLibrary::CreateRenderTarget2D(
		GetWorld(),
		RenderTarget->SizeX,
		RenderTarget->SizeY,
		RenderTarget->RenderTargetFormat
	);

	if (PhotoRT)
	{
		Photos.Add(PhotoRT);
		OnPhotoTaken.Broadcast(PhotoRT);
		G_LOG(TEXT("사진 촬영 완료: 총 %d장"), Photos.Num());
	}
}
