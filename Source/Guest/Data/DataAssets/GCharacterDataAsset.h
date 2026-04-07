// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GCharacterDataAsset.generated.h"

/**
 * 캐릭터의 이동, 카메라 등 기획 수치를 관리하는 데이터 에셋
 */
UCLASS(BlueprintType)
class GUEST_API UGCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- 이동 관련 수치 ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float CrouchSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float JumpZVelocity = 500.0f;

	// --- 카메라 관련 수치 ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float TargetZoomLength = 400.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float ZoomSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float MinZoomLength = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float MaxZoomLength = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float ZoomStep = 50.0f;
};
