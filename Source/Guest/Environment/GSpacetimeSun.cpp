// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GSpacetimeSun.h"
#include "Components/DirectionalLightComponent.h"
#include "Guest/Subsystem/GSpacetimeSubsystem.h"
#include "Kismet/GameplayStatics.h"

AGSpacetimeSun::AGSpacetimeSun()
{
	PrimaryActorTick.bCanEverTick = false;

	SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
	RootComponent = SunLight;
	
	SunLight->SetMobility(EComponentMobility::Movable);

	//SkyAtmoSphere에 태양 본체 그리게
	SunLight->bAtmosphereSunLight = true;
}

void AGSpacetimeSun::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGSpacetimeSubsystem* SpacetimeSS = GI->GetSubsystem<UGSpacetimeSubsystem>())
		{
			SpacetimeSS->OnTimeChanged.AddDynamic(this, &AGSpacetimeSun::OnTimeUpdated);
			
			OnTimeUpdated(SpacetimeSS->GetCurrentHour());
		}
	}
}

void AGSpacetimeSun::OnTimeUpdated(float CurrentHour)
{
	// 언리얼의 Pitch(Y축 회전) 기준:
	// -90 = 직각으로 내려쬠 (정오, 12시)
	// 0 = 지평선 (일출 6시)
	// -180 = 지평선 반대편 (일몰 18시)
	// 90 = 직각으로 올려쬠 (자정, 0시/24시 - 바닥 아래)
	
	// 24시간을 360도로 변환하는 선형 보간
	float CalculatedPitch = (CurrentHour / 24.0f) * -360.0f + 90.0f;

	// 계산된 Pitch 값으로 새로운 회전값 생성 (Yaw와 Roll은 0으로 고정)
	FRotator NewRotation = FRotator(CalculatedPitch, 0.0f, 0.0f);
	
	// 라이트 컴포넌트에 새로운 회전값 적용
	SunLight->SetWorldRotation(NewRotation);
}

