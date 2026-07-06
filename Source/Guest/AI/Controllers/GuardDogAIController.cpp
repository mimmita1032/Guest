// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Controllers/GuardDogAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "DrawDebugHelpers.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Data/DataAssets/GAISightDataAsset.h"
#include "Guest/Utils/GLog.h"

const FName AGuardDogAIController::BB_TargetActor(TEXT("TargetActor"));
const FName AGuardDogAIController::BB_LastKnownLocation(TEXT("LastKnownLocation"));
const FName AGuardDogAIController::BB_IsAlerted(TEXT("IsAlerted"));

AGuardDogAIController::AGuardDogAIController()
{
	// 시야 원뿔을 그릴 수 있는 빌드에서만 Tick을 허용한다.
#if ENABLE_DRAW_DEBUG
	PrimaryActorTick.bCanEverTick = true;
#else
	PrimaryActorTick.bCanEverTick = false;
#endif

	// DataAsset이 없을 때 사용할 기본 시야 설정이다.
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

#if ENABLE_DRAW_DEBUG
	SetActorTickEnabled(bDrawDebugSight);
#endif

	ApplySightSettings();
	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
		this, &AGuardDogAIController::HandleTargetPerceptionUpdated);
}

void AGuardDogAIController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if ENABLE_DRAW_DEBUG
	if (!bDrawDebugSight || !SightConfig)
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	FVector EyeLocation;
	FRotator EyeRotation;
	ControlledPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const FVector ViewDirection = EyeRotation.Vector();
	const float HalfAngleRadians = FMath::DegreesToRadians(
		SightConfig->PeripheralVisionAngleDegrees);

	// 초록색은 최초 감지, 노란색은 이미 발견한 대상을 놓치는 범위다.
	DrawDebugCone(GetWorld(), EyeLocation, ViewDirection,
		SightConfig->SightRadius, HalfAngleRadians, HalfAngleRadians,
		24, FColor::Green, false, 0.f, 0, 1.5f);

	DrawDebugCone(GetWorld(), EyeLocation, ViewDirection,
		SightConfig->LoseSightRadius, HalfAngleRadians, HalfAngleRadians,
		24, FColor::Yellow, false, 0.f, 0, 0.75f);
#endif
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
		// 새로 빙의할 때 이전 개체의 감지 상태가 남지 않도록 초기화한다.
		InitializedBlackboard->SetValueAsBool(BB_IsAlerted, false);
		InitializedBlackboard->ClearValue(BB_TargetActor);
		InitializedBlackboard->ClearValue(BB_LastKnownLocation);
	}
	
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void AGuardDogAIController::ApplySightSettings()
{
	// 맹견 종류별 DataAsset 값으로 기본 시야 설정을 덮어쓴다.
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
	// 다른 NPC나 몹은 무시하고 플레이어 캐릭터만 추적 대상으로 취급한다.
	if (!IsValid(Actor) || !Actor->IsA<AGuestCharacter>())
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		// 발견 상태: 현재 타깃을 기록하고 경계 상태로 전환한다.
		SensedPlayer = Actor;

		if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
		{
			BlackboardComp->SetValueAsObject(BB_TargetActor, Actor);
			BlackboardComp->SetValueAsBool(BB_IsAlerted, true);
			BlackboardComp->ClearValue(BB_LastKnownLocation);
		}

		G_LOG(TEXT("[GuardDog] %s - 플레이어 발견: %s"),
			*GetNameSafe(GetPawn()), *GetNameSafe(Actor));

		OnPlayerSensed(Actor);
		return;
	}

	if (SensedPlayer == Actor)
	{
		// 상실 상태: 타깃은 비우되 이후 수색을 위해 마지막 위치는 남긴다.
		SensedPlayer = nullptr;

		if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
		{
			BlackboardComp->SetValueAsVector(BB_LastKnownLocation, Stimulus.StimulusLocation);
			BlackboardComp->ClearValue(BB_TargetActor);
		}

		G_LOG(TEXT("[GuardDog] %s - 플레이어 시야 상실: %s (마지막 위치: %s)"),
			*GetNameSafe(GetPawn()),
			*GetNameSafe(Actor),
			*Stimulus.StimulusLocation.ToString());

		OnPlayerLost(Actor);
	}
}
