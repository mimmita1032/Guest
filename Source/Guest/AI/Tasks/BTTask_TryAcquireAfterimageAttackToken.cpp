// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Tasks/BTTask_TryAcquireAfterimageAttackToken.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Guest/Subsystem/GAfterimageCombatCoordinatorSubsystem.h"

UBTTask_TryAcquireAfterimageAttackToken::UBTTask_TryAcquireAfterimageAttackToken()
{
	NodeName = TEXT("Try Acquire Afterimage Attack Token");

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_TryAcquireAfterimageAttackToken, TargetActorKey), AActor::StaticClass());
	HasAttackTokenKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_TryAcquireAfterimageAttackToken, HasAttackTokenKey));
}

void UBTTask_TryAcquireAfterimageAttackToken::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BlackboardAsset = GetBlackboardAsset())
	{
		TargetActorKey.ResolveSelectedKey(*BlackboardAsset);
		HasAttackTokenKey.ResolveSelectedKey(*BlackboardAsset);
	}
}

EBTNodeResult::Type UBTTask_TryAcquireAfterimageAttackToken::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	UWorld* World = AIController->GetWorld();
	UGAfterimageCombatCoordinatorSubsystem* Coordinator = World ? World->GetSubsystem<UGAfterimageCombatCoordinatorSubsystem>() : nullptr;
	if (!Coordinator)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));

	const bool bAcquired = Coordinator->TryAcquireAttackToken(TargetActor, ControlledPawn);
	BB->SetValueAsBool(HasAttackTokenKey.SelectedKeyName, bAcquired);

	return bAcquired ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

FString UBTTask_TryAcquireAfterimageAttackToken::GetStaticDescription() const
{
	const FString TargetDesc = (TargetActorKey.SelectedKeyName != NAME_None) ? TargetActorKey.SelectedKeyName.ToString() : TEXT("(invalid)");
	const FString TokenDesc = (HasAttackTokenKey.SelectedKeyName != NAME_None) ? HasAttackTokenKey.SelectedKeyName.ToString() : TEXT("(invalid)");

	return FString::Printf(TEXT("Target: %s\nHas Attack Token: %s"), *TargetDesc, *TokenDesc);
}
