// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Tasks/BTTask_ReleaseAfterimageAttackToken.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Guest/Subsystem/GAfterimageCombatCoordinatorSubsystem.h"

UBTTask_ReleaseAfterimageAttackToken::UBTTask_ReleaseAfterimageAttackToken()
{
	NodeName = TEXT("Release Afterimage Attack Token");

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ReleaseAfterimageAttackToken, TargetActorKey), AActor::StaticClass());
	HasAttackTokenKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ReleaseAfterimageAttackToken, HasAttackTokenKey));
}

void UBTTask_ReleaseAfterimageAttackToken::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BlackboardAsset = GetBlackboardAsset())
	{
		TargetActorKey.ResolveSelectedKey(*BlackboardAsset);
		HasAttackTokenKey.ResolveSelectedKey(*BlackboardAsset);
	}
}

EBTNodeResult::Type UBTTask_ReleaseAfterimageAttackToken::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));

	if (IsValid(TargetActor))
	{
		UWorld* World = AIController->GetWorld();
		if (UGAfterimageCombatCoordinatorSubsystem* Coordinator = World ? World->GetSubsystem<UGAfterimageCombatCoordinatorSubsystem>() : nullptr)
		{
			Coordinator->ReleaseAttackToken(TargetActor, ControlledPawn);
		}
	}

	BB->SetValueAsBool(HasAttackTokenKey.SelectedKeyName, false);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_ReleaseAfterimageAttackToken::GetStaticDescription() const
{
	const FString TargetDesc = (TargetActorKey.SelectedKeyName != NAME_None) ? TargetActorKey.SelectedKeyName.ToString() : TEXT("(invalid)");
	const FString TokenDesc = (HasAttackTokenKey.SelectedKeyName != NAME_None) ? HasAttackTokenKey.SelectedKeyName.ToString() : TEXT("(invalid)");

	return FString::Printf(TEXT("Target: %s\nClear Attack Token: %s"), *TargetDesc, *TokenDesc);
}
