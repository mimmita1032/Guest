// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GDayNightManager.generated.h"

class ADirectionalLight;

UCLASS()
class GUEST_API ADGDayNightManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ADGDayNightManager();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditInstanceOnly, Category = "Environment")
	ADirectionalLight* SunLight;

	//실제 시간 계산 및 태양 회전을 담당
	void UpdateRealTimeSky();
};
