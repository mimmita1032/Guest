// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/AI/Controllers/GuestAIControllerBase.h"
#include "Guest/Data/DataAssets/GAISightDataAsset.h"
#include "GuestAIController.generated.h"

UCLASS()
class GUEST_API AGuestAIController : public AGuestAIControllerBase
{
	GENERATED_BODY()

public:
	AGuestAIController();

	// BB_TargetActor / BB_LastKnownLocation은 AGuestAIControllerBase에서 상속받는다.
	static const FName BB_TargetLocation;

	// 시간대 스케줄 관련 BB 키
	static const FName BB_ScheduledDestination; // Vector — 스케줄로 지정된 목적지
	static const FName BB_bHasSchedule;          // Bool   — 현재 시간대에 스케줄이 활성화됐는지
	static const FName BB_bScheduleArrived;      // Bool   — 스케줄 목적지에 이미 도착했는지

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

private:
	// 감지 이벤트 핸들러
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

protected:
	// 이 컨트롤러가 실행할 행동 트리
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	// 에디터에서 할당하는 시각 감지 수치 데이터 에셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<UGAISightDataAsset> SightDataAsset;
};
