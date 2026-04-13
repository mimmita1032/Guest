// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Tasks/BTTask_MarkScheduleArrived.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Guest/AI/Controllers/GuestAIController.h"

UBTTask_MarkScheduleArrived::UBTTask_MarkScheduleArrived()
{
	NodeName = TEXT("Mark Schedule Arrived");
}

EBTNodeResult::Type UBTTask_MarkScheduleArrived::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	BB->SetValueAsBool(AGuestAIController::BB_bScheduleArrived, true);
	return EBTNodeResult::Succeeded;
}
