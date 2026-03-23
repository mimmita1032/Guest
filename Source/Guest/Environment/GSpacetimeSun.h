// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GSpacetimeSun.generated.h"

class UDirectionalLightComponent;

UCLASS()
class GUEST_API AGSpacetimeSun : public AActor
{
	GENERATED_BODY()
	
public:	
	AGSpacetimeSun();

protected:
	virtual void BeginPlay() override;

	// 서브시스템의 시간이 변할 때 호출될 콜백 함수
	UFUNCTION()
	void OnTimeUpdated(float CurrentHour);

#pragma region Curves
protected:
	// 시간(0~24)에 따른 태양의 밝기(Intensity) 그래프
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacetime|Curves")
	TObjectPtr<class UCurveFloat> SunIntensityCurve;

	// 이건 달!
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacetime|Curves")
	TObjectPtr<class UCurveFloat> MoonIntensityCurve;
#pragma endregion // Curves
	
#pragma region Visuals
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	TObjectPtr<UDirectionalLightComponent> SunLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	TObjectPtr<UDirectionalLightComponent> MoonLight;
#pragma endregion // Visuals
};
