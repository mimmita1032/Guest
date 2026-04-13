// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ClearBlackboardKey.generated.h"

/**
 * 지정한 Blackboard 키를 클리어하는 범용 BT Task.
 */
UCLASS()
class GUEST_API UBTTask_ClearBlackboardKey : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ClearBlackboardKey();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	// 클리어할 Blackboard 키 (모든 타입 허용)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector KeyToClear;
};
