// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Controllers/GuestAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Guest/Characters/NPC/GuestNPCBase.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Utils/GLog.h"
#include "DrawDebugHelpers.h"

// Blackboard 키 이름 정의
const FName AGuestAIController::BB_TargetActor        = TEXT("TargetActor");
const FName AGuestAIController::BB_TargetLocation     = TEXT("TargetLocation");
const FName AGuestAIController::BB_LastKnownLocation  = TEXT("LastKnownLocation");

AGuestAIController::AGuestAIController()
{
	// SightConfig는 기본값으로 초기화 — 실제 수치는 BeginPlay에서 DataAsset으로 덮어씀
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies    = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals   = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	SetPerceptionComponent(*AIPerceptionComp);

	PrimaryActorTick.bCanEverTick = true;
}

void AGuestAIController::ApplySightDataAsset()
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

void AGuestAIController::BeginPlay()
{
	Super::BeginPlay();

	ApplySightDataAsset();

	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
			this, &AGuestAIController::HandleTargetPerceptionUpdated);
	}

	G_LOG(TEXT("[AI Sight Config] SightRadius=%.0f | LoseSightRadius=%.0f | FOV=%.0f°"),
		SightConfig->SightRadius,
		SightConfig->LoseSightRadius,
		SightConfig->PeripheralVisionAngleDegrees * 2.f);
}

void AGuestAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if ENABLE_DRAW_DEBUG
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || !SightConfig) return;

	const FVector Origin    = ControlledPawn->GetActorLocation();
	const FVector Direction = ControlledPawn->GetActorForwardVector();
	const float   HalfAngle = FMath::DegreesToRadians(SightConfig->PeripheralVisionAngleDegrees);

	// 감지 범위 (초록)
	DrawDebugCone(GetWorld(), Origin, Direction,
		SightConfig->SightRadius, HalfAngle, HalfAngle,
		16, FColor::Green, false, -1.f, 0, 1.f);

	// 시야 상실 범위 (노랑)
	DrawDebugCone(GetWorld(), Origin, Direction,
		SightConfig->LoseSightRadius, HalfAngle, HalfAngle,
		16, FColor::Yellow, false, -1.f, 0, 0.5f);

	// 현재 시야각 수치를 화면에 출력
	const FString DebugText = FString::Printf(
		TEXT("FOV: %.0f°  |  Sight: %.0fcm  |  Lose: %.0fcm"),
		SightConfig->PeripheralVisionAngleDegrees * 2.f,
		SightConfig->SightRadius,
		SightConfig->LoseSightRadius);

	DrawDebugString(GetWorld(), Origin + FVector(0.f, 0.f, 100.f), DebugText,
		nullptr, FColor::White, 0.f, true, 1.f);
#endif
}

void AGuestAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTree)
	{
		if (RunBehaviorTree(BehaviorTree))
		{
			G_LOG(TEXT("AI 행동 트리 작동 시작: %s"), *InPawn->GetName());
		}
		else
		{
			G_ERR(TEXT("행동 트리 실행 실패!"));
		}
	}
	else
	{
		G_WARN(TEXT("BehaviorTree가 설정되지 않았습니다. (Controller: %s)"), *GetName());
	}
}

void AGuestAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 플레이어 캐릭터만 처리
	if (!Cast<AGuestCharacter>(Actor)) return;

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		// 플레이어 발견 -> TargetActor 설정
		BB->SetValueAsObject(BB_TargetActor, Actor);
		G_LOG(TEXT("플레이어 감지: %s"), *Actor->GetName());
	}
	else
	{
		// 시야 이탈 -> 마지막 위치 저장 후 TargetActor 클리어
		BB->SetValueAsVector(BB_LastKnownLocation, Actor->GetActorLocation());
		BB->ClearValue(BB_TargetActor);
		G_LOG(TEXT("플레이어 시야 이탈 — 마지막 위치: %s"), *Actor->GetActorLocation().ToString());
	}
}
