// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_OrientToTarget.generated.h"

/**
 * 포위(Strafing) Behavior Tree 브랜치 전용 회전 서비스.
 * 매 Tick 지정된 Blackboard의 TargetActor 방향을 바라보도록 Pawn을 SetActorRotation()으로 직접 회전시킨다.
 * CharacterMovement의 bOrientRotationToMovement 등 다른 회전 옵션은 건드리지 않으며,
 * 이 서비스가 부착된 브랜치가 활성화된 동안에만 타깃 방향 보정을 추가로 수행한다.
 */
UCLASS()
class GUEST_API UBTService_OrientToTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_OrientToTarget();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	// 회전 기준이 되는 타깃 Actor Blackboard 키 (Actor 타입만 선택 가능)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector InTargetActorKey;

	// Yaw 회전 보간 속도 (FMath::RInterpTo에 사용)
	UPROPERTY(EditAnywhere, Category = "Rotation", meta = (ClampMin = "0.0"))
	float RotationInterpSpeed = 5.0f;
};