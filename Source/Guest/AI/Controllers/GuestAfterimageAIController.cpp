// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Controllers/GuestAfterimageAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Guest/Characters/Enemy/GuestEnemyCharacter.h"
#include "Guest/AI/GuestTeamIds.h"
#include "Guest/Subsystem/GAfterimageCombatCoordinatorSubsystem.h"
#include "Guest/Utils/GLog.h"

// Blackboard 키 이름 정의 — BB_TargetActor는 AGuestAIControllerBase에서 정의한다.
const FName AGuestAfterimageAIController::BB_StrafeLocation = TEXT("StrafeLocation");
const FName AGuestAfterimageAIController::BB_HasAttackToken = TEXT("HasAttackToken");

AGuestAfterimageAIController::AGuestAfterimageAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	// 이 컨트롤러(Listener)의 팀을 Enemy로 고정 — GetTeamAttitudeTowards() 판정의 기준이 된다
	SetGenericTeamId(FGenericTeamId(GuestTeamIds::Enemy));

	// SightConfig/AIPerceptionComp 객체 자체는 Base 생성자가 만든다.
	// 잔상 Sight는 Hostile 판정 대상만 감지
	SightConfig->DetectionByAffiliation.bDetectEnemies    = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals   = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	FinalizeSightPerceptionSetup();
}

ETeamAttitude::Type AGuestAfterimageAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	// 감지 대상은 Pawn이므로 Controller를 거쳐 Team을 조회한다
	const APawn* OtherPawn = Cast<const APawn>(&Other);
	const AController* OtherController = OtherPawn ? OtherPawn->GetController() : nullptr;
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(OtherController);
	if (!OtherTeamAgent)
	{
		return ETeamAttitude::Neutral;
	}

	const FGenericTeamId MyTeamId    = GetGenericTeamId();
	const FGenericTeamId OtherTeamId = OtherTeamAgent->GetGenericTeamId();

	if (MyTeamId == FGenericTeamId::NoTeam || OtherTeamId == FGenericTeamId::NoTeam)
	{
		return ETeamAttitude::Neutral;
	}

	return MyTeamId == OtherTeamId ? ETeamAttitude::Friendly : ETeamAttitude::Hostile;
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
	CrowdComp->SetCrowdCollisionQueryRange(CollisionQueryRange, false);

	// 잔상 Crowd Agent끼리만 서로 회피하도록 그룹 지정 (플레이어/현실 적은 Crowd Agent가 아니므로 영향 없음)
	CrowdComp->SetAvoidanceGroup(AfterimageCrowdGroup, false);
	CrowdComp->SetGroupsToAvoid(AfterimageCrowdGroup, false);
	CrowdComp->SetGroupsToIgnore(0, false);

	// Crowd가 이동 속도 방향으로 캐릭터를 강제 회전하지 않도록 설정 — 배회/이동 회전은 bOrientRotationToMovement, 포위 중 타깃 보정은 BTS_OrientToTarget이 전담
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
	ApplySightDataAssetToConfig(SightDataAsset);
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

	// 공격권 관련 Blackboard 값 초기화
	BB->SetValueAsBool(BB_HasAttackToken, false);

	G_LOG(TEXT("잔상 AI 행동 트리 작동 시작: %s"), *InPawn->GetName());

	// 4) Perception 델리게이트 등록 — 재빙의(재Possess) 시 중복 방지
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddUniqueDynamic(
			this, &AGuestAfterimageAIController::HandleTargetPerceptionUpdated);

		// 5) Blackboard와 델리게이트가 모두 준비된 뒤 리스너 갱신 요청
		AIPerceptionComp->RequestStimuliListenerUpdate();
	}

	// 6) 알람 반경 검색 대상으로 Coordinator에 등록
	if (UWorld* World = GetWorld())
	{
		if (UGAfterimageCombatCoordinatorSubsystem* Coordinator = World->GetSubsystem<UGAfterimageCombatCoordinatorSubsystem>())
		{
			Coordinator->RegisterAfterimageController(this);
		}
	}
}

void AGuestAfterimageAIController::OnUnPossess()
{
	// Super::OnUnPossess()가 Pawn 참조를 해제하기 전에 토큰 반환/등록 해제를 마쳐야 한다
	ReleaseAttackTokenIfHeld();

	if (UWorld* World = GetWorld())
	{
		if (UGAfterimageCombatCoordinatorSubsystem* Coordinator = World->GetSubsystem<UGAfterimageCombatCoordinatorSubsystem>())
		{
			Coordinator->UnregisterAfterimageController(this);
		}
	}

	Super::OnUnPossess();
}

void AGuestAfterimageAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!IsValid(Actor))
	{
		return;
	}

	// 회전(Focus 유사 역할)은 Behavior Tree의 BTS_OrientToTarget 서비스가 TargetActor 키를 읽어 직접 처리한다
	const bool bSuccessfullySensed = Stimulus.WasSuccessfullySensed();

	if (bSuccessfullySensed)
	{
		// Sight Affiliation 필터를 이미 통과했지만 방어적으로 한 번 더 확인한다
		if (GetTeamAttitudeTowards(*Actor) != ETeamAttitude::Hostile)
		{
			return;
		}
	}

	UWorld* World = GetWorld();
	UGAfterimageCombatCoordinatorSubsystem* Coordinator = World ? World->GetSubsystem<UGAfterimageCombatCoordinatorSubsystem>() : nullptr;
	if (Coordinator)
	{
		// 그룹 전투 유지/해제 판단은 Coordinator가 DirectObservers 기준으로 수행한다
		Coordinator->ReportTargetSensed(this, Actor, bSuccessfullySensed, AlertRadius);
		return;
	}

	// 예외적인 초기화 실패 상황 — 최소한 자기 Blackboard만 안전하게 정리하는 fallback
	G_WARN(TEXT("AfterimageCombatCoordinatorSubsystem을 찾을 수 없습니다. 개별 Blackboard만 갱신합니다. (Controller: %s)"), *GetName());

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	if (bSuccessfullySensed)
	{
		BB->SetValueAsObject(BB_TargetActor, Actor);
	}
	else if (BB->GetValueAsObject(BB_TargetActor) == Actor)
	{
		ReleaseAttackTokenIfHeld();
		BB->ClearValue(BB_TargetActor);
	}
}

void AGuestAfterimageAIController::ReceiveSharedCombatTarget(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return;
	}

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	BB->SetValueAsObject(BB_TargetActor, TargetActor);
}

void AGuestAfterimageAIController::ClearSharedCombatTarget(AActor* TargetActor)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	// 이미 다른 Target으로 전환된 경우 그 값을 지우지 않는다
	if (BB->GetValueAsObject(BB_TargetActor) != TargetActor)
	{
		return;
	}

	ReleaseAttackTokenIfHeld();
	BB->ClearValue(BB_TargetActor);
}

void AGuestAfterimageAIController::ReleaseAttackTokenIfHeld()
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	if (!BB->GetValueAsBool(BB_HasAttackToken))
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(BB_TargetActor));
	APawn* ControlledPawn = GetPawn();

	if (IsValid(TargetActor) && IsValid(ControlledPawn))
	{
		if (UWorld* World = GetWorld())
		{
			if (UGAfterimageCombatCoordinatorSubsystem* Coordinator = World->GetSubsystem<UGAfterimageCombatCoordinatorSubsystem>())
			{
				Coordinator->ReleaseAttackToken(TargetActor, ControlledPawn);
			}
		}
	}

	BB->SetValueAsBool(BB_HasAttackToken, false);
}