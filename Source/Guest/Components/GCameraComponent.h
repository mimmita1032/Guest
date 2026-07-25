// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GCameraComponent.generated.h"

class UTextureRenderTarget2D;
class USceneCaptureComponent2D;
class UKismetRenderingLibrary;
class UGItemDefinition;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUEST_API UGCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGCameraComponent();

	// 촬영 → 인벤토리에 사진 아이템 지급. 인벤토리에 공간이 없으면 촬영이 실패한다.
	// 성공 여부를 반환하므로 UI에서 실패 피드백을 띄울 수 있다.
	UFUNCTION(BlueprintCallable, Category = "Camera")
	bool TakePhoto(const FPhotoData& Metadata);

	UFUNCTION(BlueprintPure, Category = "Camera")
	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

	// Camera 탭에서 구독
	UPROPERTY(BlueprintAssignable, Category = "Camera")
	FOnPhotoTaken OnPhotoTaken;

	void SetupCapture(USceneCaptureComponent2D* InCapture, UTextureRenderTarget2D* InRenderTarget);

protected:
	virtual void BeginPlay() override;

	// 모든 사진이 공유하는 설계도 (에디터에서 DA_Item_Photo 지정)
	// 개체마다 다른 내용은 FGPhotoItemInstanceData로 들어간다
	UPROPERTY(EditDefaultsOnly, Category = "Camera|Photo")
	TObjectPtr<const UGItemDefinition> PhotoItemDefinition;

private:
	UPROPERTY()
	TObjectPtr<USceneCaptureComponent2D> CaptureComponent;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;
};
