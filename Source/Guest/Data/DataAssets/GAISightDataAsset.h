// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GAISightDataAsset.generated.h"

/**
 * NPC AI 시각 감지 수치를 관리하는 데이터 에셋
 */
UCLASS(BlueprintType)
class GUEST_API UGAISightDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 플레이어를 감지하는 최대 거리 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sight", meta = (ClampMin = "0.0"))
	float SightRadius = 1500.f;

	// 이 거리 밖으로 나가야 시야를 잃는 거리 (cm, SightRadius보다 커야 함)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sight", meta = (ClampMin = "0.0"))
	float LoseSightRadius = 1800.f;

	// 좌우 시야 반각 (도). 전체 시야각 = 이 값 × 2
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sight", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float PeripheralVisionAngleDegrees = 60.f;

	// 감지 자극 유효 시간 (초). 이 시간이 지나면 자극이 만료됨
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sight", meta = (ClampMin = "0.0"))
	float MaxAge = 5.f;
};
