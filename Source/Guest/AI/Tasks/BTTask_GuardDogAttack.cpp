// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Tasks/BTTask_GuardDogAttack.h"

#include "AIController.h"
#include "Guest/Characters/Enemy/GuardDogCharacter.h"

UBTTask_GuardDogAttack::UBTTask_GuardDogAttack()
{
	NodeName = TEXT("Guard Dog Attack");
}

EBTNodeResult::Type UBTTask_GuardDogAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AGuardDogCharacter* GuardDog = Cast<AGuardDogCharacter>(AIController->GetPawn());
	if (!GuardDog)
	{
		return EBTNodeResult::Failed;
	}

	return GuardDog->RequestAttack() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
