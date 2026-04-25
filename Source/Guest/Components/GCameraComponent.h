// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GCameraComponent.generated.h"

class UTextureRenderTarget2D;
class USceneCaptureComponent2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhotoTaken, UTextureRenderTarget2D*, Photo);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUEST_API UGCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGCameraComponent();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void TakePhoto();

	UFUNCTION(BlueprintPure, Category = "Camera")
	const TArray<UTextureRenderTarget2D*>& GetPhotos() const { return Photos; }

	UFUNCTION(BlueprintPure, Category = "Camera")
	int32 GetPhotoCount() const { return Photos.Num(); }

	// Camera 탭에서 구독
	UPROPERTY(BlueprintAssignable, Category = "Camera")
	FOnPhotoTaken OnPhotoTaken;

protected:
	virtual void BeginPlay() override;

	// BP에서 할당할 기본 렌더 타겟 (해상도/포맷 템플릿 역할)
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USceneCaptureComponent2D> CaptureComponent;

private:
	UPROPERTY()
	TArray<TObjectPtr<UTextureRenderTarget2D>> Photos;
};
