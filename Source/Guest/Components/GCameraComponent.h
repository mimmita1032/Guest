// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GCameraComponent.generated.h"

class UTextureRenderTarget2D;
class USceneCaptureComponent2D;
class UKismetRenderingLibrary;

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
	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

	UFUNCTION(BlueprintPure, Category = "Camera")
	const TArray<UTextureRenderTarget2D*>& GetPhotos() const { return Photos; }

	UFUNCTION(BlueprintPure, Category = "Camera")
	int32 GetPhotoCount() const { return Photos.Num(); }

	// Camera 탭에서 구독
	UPROPERTY(BlueprintAssignable, Category = "Camera")
	FOnPhotoTaken OnPhotoTaken;

	void SetupCapture(USceneCaptureComponent2D* InCapture, UTextureRenderTarget2D* InRenderTarget);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<USceneCaptureComponent2D> CaptureComponent;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY()
	TArray<TObjectPtr<UTextureRenderTarget2D>> Photos;
};
