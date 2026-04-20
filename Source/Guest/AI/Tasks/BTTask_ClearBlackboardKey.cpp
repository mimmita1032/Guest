// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Tasks/BTTask_ClearBlackboardKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearBlackboardKey::UBTTask_ClearBlackboardKey()
{
	NodeName = TEXT("Clear Blackboard Key");
}

EBTNodeResult::Type UBTTask_ClearBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	BB->ClearValue(KeyToClear.SelectedKeyName);
	return EBTNodeResult::Succeeded;
}
