// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GuardDogAttack.generated.h"

/**
 * Behavior Tree task that asks the possessed guard dog to perform its bite attack.
 */
UCLASS()
class GUEST_API UBTTask_GuardDogAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GuardDogAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
