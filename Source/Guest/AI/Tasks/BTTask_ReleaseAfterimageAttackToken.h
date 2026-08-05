// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ReleaseAfterimageAttackToken.generated.h"

/**
 * 잔상 포위 전투의 최소 공격권을 Coordinator에 반환하는 즉시 완료형 BT Task.
 * TargetActor가 이미 무효화되어 있어도 HasAttackTokenKey는 반드시 false로 정리하고 Succeeded를 반환한다.
 */
UCLASS()
class GUEST_API UBTTask_ReleaseAfterimageAttackToken : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ReleaseAfterimageAttackToken();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	// 공격권을 반환할 대상(플레이어) Blackboard 키 (Actor 타입만 선택 가능)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	// 반환 후 정리할 Blackboard 키 (Bool 타입)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HasAttackTokenKey;
};
