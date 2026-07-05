// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Controllers/GuardDogAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Data/DataAssets/GAISightDataAsset.h"

const FName AGuardDogAIController::BB_TargetActor(TEXT("TargetActor"));
const FName AGuardDogAIController::BB_LastKnownLocation(TEXT("LastKnownLocation"));
const FName AGuardDogAIController::BB_IsAlerted(TEXT("IsAlerted"));

AGuardDogAIController::AGuardDogAIController()
{
	PrimaryActorTick.bCanEverTick = false;

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 1800.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->SetMaxAge(2.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	SetPerceptionComponent(*AIPerceptionComp);
}

void AGuardDogAIController::BeginPlay()
{
	Super::BeginPlay();

	ApplySightSettings();
	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
		this, &AGuardDogAIController::HandleTargetPerceptionUpdated);
}

void AGuardDogAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!BlackboardAsset)
	{
		return;
	}

	UBlackboardComponent* InitializedBlackboard = nullptr;
	if (UseBlackboard(BlackboardAsset, InitializedBlackboard) && InitializedBlackboard)
	{
		InitializedBlackboard->SetValueAsBool(BB_IsAlerted, false);
		InitializedBlackboard->ClearValue(BB_TargetActor);
		InitializedBlackboard->ClearValue(BB_LastKnownLocation);
	}
}

void AGuardDogAIController::ApplySightSettings()
{
	if (SightDataAsset)
	{
		SightConfig->SightRadius = SightDataAsset->SightRadius;
		SightConfig->LoseSightRadius = FMath::Max(
			SightDataAsset->LoseSightRadius, SightDataAsset->SightRadius);
		SightConfig->PeripheralVisionAngleDegrees = SightDataAsset->PeripheralVisionAngleDegrees;
		SightConfig->SetMaxAge(SightDataAsset->MaxAge);
	}

	AIPerceptionComp->ConfigureSense(*SightConfig);
}

void AGuardDogAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!IsValid(Actor) || !Actor->IsA<AGuestCharacter>())
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		SensedPlayer = Actor;

		if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
		{
			BlackboardComp->SetValueAsObject(BB_TargetActor, Actor);
			BlackboardComp->SetValueAsBool(BB_IsAlerted, true);
			BlackboardComp->ClearValue(BB_LastKnownLocation);
		}

		OnPlayerSensed(Actor);
		return;
	}

	if (SensedPlayer == Actor)
	{
		SensedPlayer = nullptr;

		if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
		{
			BlackboardComp->SetValueAsVector(BB_LastKnownLocation, Stimulus.StimulusLocation);
			BlackboardComp->ClearValue(BB_TargetActor);
		}

		OnPlayerLost(Actor);
	}
}
