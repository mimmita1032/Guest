// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "GuestEnvQueryContext_CombatTarget.generated.h"

/**
 * UGuestEnvQueryContext_CombatTarget
 *
 * 잔상 Strafing EQS Generator(Donut)의 중심점으로 사용할 컨텍스트.
 * Querier(잔상 Pawn) → AIController → Blackboard의 TargetActor 키를 읽어
 * 현재 전투 타깃 Actor를 EQS 컨텍스트 데이터로 제공한다.
 */
UCLASS()
class GUEST_API UGuestEnvQueryContext_CombatTarget : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};