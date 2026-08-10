// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Controllers/GuestAIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Guest/Data/DataAssets/GAISightDataAsset.h"
#include "Guest/Utils/GLog.h"

const FName AGuestAIControllerBase::BB_TargetActor       = TEXT("TargetActor");
const FName AGuestAIControllerBase::BB_LastKnownLocation = TEXT("LastKnownLocation");

AGuestAIControllerBase::AGuestAIControllerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 세부값(DetectionByAffiliation, SightRadius 등)은 파생 클래스 생성자가 채운다.
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AGuestAIControllerBase::FinalizeSightPerceptionSetup()
{
	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	SetPerceptionComponent(*AIPerceptionComp);
}

void AGuestAIControllerBase::ApplySightDataAssetToConfig(const UGAISightDataAsset* InSightDataAsset)
{
	if (!InSightDataAsset)
	{
		G_WARN(TEXT("SightDataAsset이 설정되지 않았습니다. 기본값을 사용합니다."));
		return;
	}

	SightConfig->SightRadius                  = InSightDataAsset->SightRadius;
	SightConfig->LoseSightRadius              = InSightDataAsset->LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = InSightDataAsset->PeripheralVisionAngleDegrees;
	SightConfig->SetMaxAge(InSightDataAsset->MaxAge);

	// DetectionByAffiliation은 생성자에서 이미 설정된 값을 그대로 포함해 다시 스냅샷한다.
	AIPerceptionComp->ConfigureSense(*SightConfig);
}