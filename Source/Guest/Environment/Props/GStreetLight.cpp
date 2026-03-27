// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GStreetLight.h"
#include "Components/PointLightComponent.h"
#include "Guest/Subsystem/GSpacetimeSubsystem.h"
#include "Guest/Utils/GLog.h"

AGStreetLight::AGStreetLight()
{
	PrimaryActorTick.bCanEverTick = false;

	LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("StreetLight"));
	SetRootComponent(LightComponent);
	
	LightComponent->SetIntensity(0.0f);
}

void AGStreetLight::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGSpacetimeSubsystem* TimeSubsystem = GI->GetSubsystem<UGSpacetimeSubsystem>())
		{
			TimeSubsystem->OnTimeChanged.AddDynamic(this, &AGStreetLight::HandleTimeChanged);
			HandleTimeChanged(TimeSubsystem->GetCurrentHour());
		}
	}
}

void AGStreetLight::HandleTimeChanged(float CurrentHour)
{
	OnTimeChanged(CurrentHour);
}

void AGStreetLight::OnTimeChanged(float CurrentHour)
{
	const bool bShouldBeOn = (CurrentHour >= TurnOnTime || CurrentHour <= TurnOffTime);
	const float TargetIntensity = bShouldBeOn ? 5000.0f : 0.0f;

	if (LightComponent->Intensity != TargetIntensity)
	{
		LightComponent->SetIntensity(TargetIntensity);
		G_LOG(TEXT("가로등 상태 변경 - 현재 시간: %.2f, 전등: %s"), CurrentHour, bShouldBeOn ? TEXT("켜짐") : TEXT("꺼짐"));
	}
}

