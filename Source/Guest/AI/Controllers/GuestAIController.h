// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Guest/Data/DataAssets/GAISightDataAsset.h"
#include "GuestAIController.generated.h"

UCLASS()
class GUEST_API AGuestAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGuestAIController();

	// Blackboard 키 이름 상수 (BT Task 등 외부에서도 참조 가능)
	static const FName BB_TargetActor;
	static const FName BB_TargetLocation;
	static const FName BB_LastKnownLocation;

	// 시간대 스케줄 관련 BB 키
	static const FName BB_ScheduledDestination; // Vector — 스케줄로 지정된 목적지
	static const FName BB_bHasSchedule;          // Bool   — 현재 시간대에 스케줄이 활성화됐는지
	static const FName BB_bScheduleArrived;      // Bool   — 스케줄 목적지에 이미 도착했는지

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

private:
	// SightDataAsset 수치를 SightConfig에 적용하고 PerceptionComponent를 갱신
	void ApplySightDataAsset();

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

	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
};
