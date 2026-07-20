// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Controllers/GuestAfterimageAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Guest/Characters/Enemy/GuestEnemyCharacter.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Utils/GLog.h"

// Blackboard 키 이름 정의
const FName AGuestAfterimageAIController::BB_TargetActor    = TEXT("TargetActor");
const FName AGuestAfterimageAIController::BB_StrafeLocation = TEXT("StrafeLocation");

AGuestAfterimageAIController::AGuestAfterimageAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies    = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals   = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	SetPerceptionComponent(*AIPerceptionComp);
}

void AGuestAfterimageAIController::ApplySightDataAsset()
{
	if (!SightDataAsset)
	{
		G_WARN(TEXT("SightDataAsset이 설정되지 않았습니다. 기본값을 사용합니다."));
		return;
	}

	SightConfig->SightRadius                  = SightDataAsset->SightRadius;
	SightConfig->LoseSightRadius              = SightDataAsset->LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = SightDataAsset->PeripheralVisionAngleDegrees;
	SightConfig->SetMaxAge(SightDataAsset->MaxAge);

	// 변경된 수치를 PerceptionComponent에 반영
	AIPerceptionComp->ConfigureSense(*SightConfig);
}

void AGuestAfterimageAIController::ApplyCrowdSettings()
{
	UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent());
	if (!CrowdComp)
	{
		G_ERR(TEXT("CrowdFollowingComponent 캐스팅 실패 — PathFollowingComponent 타입을 확인하세요. (Controller: %s)"), *GetName());
		return;
	}

	// Simulation On/Off는 Agent 등록을 직접 좌우하므로 배치 설정과 분리해 즉시 적용한다
	CrowdComp->SetCrowdSimulationState(bEnableCrowdSimulation ? ECrowdSimulationState::Enabled : ECrowdSimulationState::Disabled);

	// 아래는 bUpdateAgent=false로 값만 갱신한 뒤 UpdateCrowdAgentParams()에서 한 번에 반영한다
	CrowdComp->SetCrowdObstacleAvoidance(bEnableCrowdAvoidance, false);
	CrowdComp->SetCrowdAvoidanceQuality(static_cast<ECrowdAvoidanceQuality::Type>(CrowdAvoidanceQuality), false);
	CrowdComp->SetCrowdSeparation(bEnableCrowdSeparation, false);
	CrowdComp->SetCrowdSeparationWeight(CrowdSeparationWeight, false);

	// 잔상 Crowd Agent끼리만 서로 회피하도록 그룹 지정 (플레이어/현실 적은 Crowd Agent가 아니므로 영향 없음)
	CrowdComp->SetAvoidanceGroup(AfterimageCrowdGroup, false);
	CrowdComp->SetGroupsToAvoid(AfterimageCrowdGroup, false);
	CrowdComp->SetGroupsToIgnore(0, false);

	// Crowd가 이동 속도 방향으로 캐릭터를 강제 회전하지 않도록 설정 — 회전은 BTS_RotateToTarget이 전담
	CrowdComp->SetCrowdRotateToVelocity(bCrowdRotateToVelocity);

	CrowdComp->UpdateCrowdAgentParams();
}

void AGuestAfterimageAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 1) 잔상 타입 Pawn 검증 — 현실 타입이나 다른 Pawn을 Possess하면 초기화를 중단한다
	AGuestEnemyCharacter* AfterimageCharacter = Cast<AGuestEnemyCharacter>(InPawn);
	if (!AfterimageCharacter || !AfterimageCharacter->IsAfterimageEnemy())
	{
		G_ERR(TEXT("잔상 AIController가 잔상(AfterImage) 타입이 아닌 Pawn을 Possess했습니다: %s"),
			InPawn ? *InPawn->GetName() : TEXT("nullptr"));
		return;
	}

	G_LOG(TEXT("잔상 AIController Possess: %s"), *InPawn->GetName());

	// 2) Sight / Crowd / RVO 설정
	ApplySightDataAsset();
	ApplyCrowdSettings();

	if (UCharacterMovementComponent* MoveComp = AfterimageCharacter->GetCharacterMovement())
	{
		// RVO와 Detour Crowd(Crowd Avoidance)를 동시에 사용하지 않도록 비활성화
		MoveComp->bUseRVOAvoidance = false;
	}

	// 3) BehaviorTree 실행 및 Blackboard 초기화 확인 — 실패 시 Perception 처리를 시작하지 않는다
	if (!BehaviorTree)
	{
		G_WARN(TEXT("BehaviorTree가 설정되지 않았습니다. (Controller: %s)"), *GetName());
		return;
	}

	if (!RunBehaviorTree(BehaviorTree))
	{
		G_ERR(TEXT("잔상 행동 트리 실행 실패! Perception 초기화를 건너뜁니다. (Controller: %s)"), *GetName());
		return;
	}

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB)
	{
		G_ERR(TEXT("행동 트리 실행 후에도 Blackboard가 초기화되지 않았습니다. (Controller: %s)"), *GetName());
		return;
	}

	G_LOG(TEXT("잔상 AI 행동 트리 작동 시작: %s"), *InPawn->GetName());

	// 4) Perception 델리게이트 등록 — 재빙의(재Possess) 시 중복 방지
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddUniqueDynamic(
			this, &AGuestAfterimageAIController::HandleTargetPerceptionUpdated);

		// 5) Blackboard와 델리게이트가 모두 준비된 뒤 리스너 갱신 요청
		AIPerceptionComp->RequestStimuliListenerUpdate();
	}
}

void AGuestAfterimageAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 플레이어 캐릭터만 전투 타깃으로 처리
	if (!Cast<AGuestCharacter>(Actor)) return;

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB)
	{
		G_WARN(TEXT("HandleTargetPerceptionUpdated: Blackboard가 아직 초기화되지 않았습니다."));
		return;
	}

	// 회전(Focus 유사 역할)은 Behavior Tree의 BTS_RotateToTarget 서비스가 이 키를 읽어 직접 처리한다
	if (Stimulus.WasSuccessfullySensed() && IsValid(Actor))
	{
		BB->SetValueAsObject(BB_TargetActor, Actor);
		G_LOG(TEXT("잔상 전투 타깃 감지: %s"), *Actor->GetName());
	}
	else
	{
		BB->ClearValue(BB_TargetActor);
		G_LOG(TEXT("잔상 전투 타깃 상실: %s"), *Actor->GetName());
	}
}