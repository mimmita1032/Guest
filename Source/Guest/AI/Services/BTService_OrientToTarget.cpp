// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Services/BTService_OrientToTarget.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_OrientToTarget::UBTService_OrientToTarget()
{
	NodeName = TEXT("Orient Rotation to Target Actor");

	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	Interval = 0.f;
	RandomDeviation = 0.f;
	RotationInterpSpeed = 5.f;

	// 에디터에서 Actor 타입 Blackboard 키만 선택 가능하도록 필터
	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_OrientToTarget, InTargetActorKey), AActor::StaticClass());
}

void UBTService_OrientToTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BlackboardAsset = GetBlackboardAsset())
	{
		InTargetActorKey.ResolveSelectedKey(*BlackboardAsset);
	}
}

void UBTService_OrientToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!Blackboard || !AIController) return;

	APawn* OwningPawn = AIController->GetPawn();
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(InTargetActorKey.SelectedKeyName));

	if (!IsValid(OwningPawn) || !IsValid(TargetActor)) return;

	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
		OwningPawn->GetActorLocation(), TargetActor->GetActorLocation());

	const FRotator TargetRotation(0.f, LookAtRotation.Yaw, 0.f);

	const FRotator NewRotation = FMath::RInterpTo(
		OwningPawn->GetActorRotation(), TargetRotation, DeltaSeconds, RotationInterpSpeed);

	OwningPawn->SetActorRotation(NewRotation);
}

FString UBTService_OrientToTarget::GetStaticDescription() const
{
	const FString KeyDesc = (InTargetActorKey.SelectedKeyName != NAME_None)
		? InTargetActorKey.SelectedKeyName.ToString()
		: TEXT("(invalid)");

	return FString::Printf(TEXT("Target: %s\nInterp Speed: %.1f"), *KeyDesc, RotationInterpSpeed);
}