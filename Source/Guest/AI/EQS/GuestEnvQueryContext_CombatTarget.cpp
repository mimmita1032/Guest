// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/EQS/GuestEnvQueryContext_CombatTarget.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Guest/AI/Controllers/GuestAfterimageAIController.h"
#include "Guest/Utils/GLog.h"

void UGuestEnvQueryContext_CombatTarget::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	// 1) Querier가 Pawn인지 확인
	const APawn* QuerierPawn = Cast<APawn>(QueryInstance.Owner.Get());
	if (!QuerierPawn)
	{
		G_WARN(TEXT("CombatTarget Context: Querier가 Pawn이 아닙니다."));
		return;
	}

	// 2) AIController의 Blackboard 획득
	const AAIController* AIC = Cast<AAIController>(QuerierPawn->GetController());
	if (!AIC)
	{
		G_WARN(TEXT("CombatTarget Context: AIController를 가져올 수 없습니다."));
		return;
	}

	const UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB)
	{
		G_WARN(TEXT("CombatTarget Context: Blackboard가 아직 초기화되지 않았습니다."));
		return;
	}

	// 3) 기존 TargetActor Blackboard 키 읽기
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(AGuestAfterimageAIController::BB_TargetActor));
	if (!IsValid(TargetActor))
	{
		// 5) 유효한 타깃이 없으면 빈 결과 반환 — EQS가 쿼리를 실패 처리하도록 둔다
		return;
	}

	// 4) 유효한 Actor를 Context 데이터로 제공
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
}