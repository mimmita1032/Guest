// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TryAcquireAfterimageAttackToken.generated.h"

/**
 * 잔상 포위 전투의 최소 공격권을 Coordinator로부터 획득 시도하는 즉시 완료형 BT Task.
 * 성공 시 HasAttackTokenKey를 true로 설정하고 Succeeded, 실패 시 false로 설정하고 Failed를 반환한다.
 * Move To, 공격 실행, Gameplay Ability 활성화, 애니메이션, 상태 enum 변경, Wait 중 어느 것도 수행하지 않는다.
 */
UCLASS()
class GUEST_API UBTTask_TryAcquireAfterimageAttackToken : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TryAcquireAfterimageAttackToken();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	// 공격권을 요청할 대상(플레이어) Blackboard 키 (Actor 타입만 선택 가능)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	// 획득 결과를 저장할 Blackboard 키 (Bool 타입)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HasAttackTokenKey;
};
