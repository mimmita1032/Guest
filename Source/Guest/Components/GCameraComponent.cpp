// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Guest/Utils/GLog.h"

UGCameraComponent::UGCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->bCaptureOnMovement = false;
}

void UGCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (CaptureComponent && RenderTarget)
	{
		CaptureComponent->TextureTarget = RenderTarget;
	}
}

void UGCameraComponent::TakePhoto()
{
	if (!CaptureComponent || !RenderTarget)
	{
		G_WARN(TEXT("카메라: RenderTarget 또는 CaptureComponent 미설정"));
		return;
	}

	// 현재 프레임 캡처
	CaptureComponent->CaptureScene();

	// 찍힌 렌더 타겟을 복사해 저장 (UI에서 UTextureRenderTarget2D로 직접 표시)
	UTextureRenderTarget2D* PhotoRT = UKismetRenderingLibrary::CreateRenderTarget2D(
		GetWorld(),
		RenderTarget->SizeX,
		RenderTarget->SizeY,
		RenderTarget->RenderTargetFormat
	);

	if (PhotoRT)
	{
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), PhotoRT, nullptr);
		Photos.Add(PhotoRT);
		OnPhotoTaken.Broadcast(PhotoRT);
		G_LOG(TEXT("사진 촬영 완료: 총 %d장"), Photos.Num());
	}
}
