// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindRandomLocation.generated.h"

/**
 * NavMesh 위의 랜덤 위치를 찾아 Blackboard에 저장하는 BT Task.
 * Patrol(순찰) 시퀀스에서 목적지 선택에 사용한다.
 */
UCLASS()
class GUEST_API UBTTask_FindRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindRandomLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	// 결과 위치를 저장할 Blackboard 키 (Vector 타입)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;

	// NPC 현재 위치 기준 탐색 반경 (cm)
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "100.0"))
	float SearchRadius = 1000.f;
};
