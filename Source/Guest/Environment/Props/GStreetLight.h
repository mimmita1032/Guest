// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Guest/Interfaces/GTimeListenerInterface.h"
#include "GStreetLight.generated.h"

class UPointLightComponent;

UCLASS()
class GUEST_API AGStreetLight : public AActor, public IGTimeListenerInterface
{
	GENERATED_BODY()

public:	
	AGStreetLight();

protected:
	virtual void BeginPlay() override;

	virtual void OnTimeChanged(float CurrentHour) override;

	UFUNCTION()
	void HandleTimeChanged(float CurrentHour);

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPointLightComponent> LightComponent;

	//불 켜고 꺼지는 시간
	UPROPERTY(EditAnywhere, Category = "Settings")
	float TurnOnTime = 18.0f;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float TurnOffTime = 6.0f;
};
