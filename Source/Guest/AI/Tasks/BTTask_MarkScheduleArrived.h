// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MarkScheduleArrived.generated.h"

/**
 * 스케줄 목적지에 도착했음을 BB에 기록하는 BT Task.
 * MoveTo(ScheduledDestination) 성공 직후에 배치한다.
 * bScheduleArrived = true 로 설정해 다음 스케줄 변경 전까지 재이동을 막는다.
 */
UCLASS()
class GUEST_API UBTTask_MarkScheduleArrived : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MarkScheduleArrived();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
