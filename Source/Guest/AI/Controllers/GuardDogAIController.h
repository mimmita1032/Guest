// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/AI/Controllers/GuestAIControllerBase.h"
#include "GuardDogAIController.generated.h"

class UGAISightDataAsset;
class UBlackboardData;

/**
 * 맹견 전용 AI Controller.
 * 시야 감지 결과를 Blackboard에 기록하고 디버그 정보를 제공한다.
 */
UCLASS()
class GUEST_API AGuardDogAIController : public AGuestAIControllerBase
{
	GENERATED_BODY()

public:
	AGuardDogAIController();

	// BB_TargetActor / BB_LastKnownLocation은 AGuestAIControllerBase에서 상속받는다.
	static const FName BB_IsAlerted;

	UFUNCTION(BlueprintPure, Category = "Guest|GuardDog|Perception")
	AActor* GetSensedPlayer() const { return SensedPlayer; }

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	/** 디버그 빌드에서 최초 감지/시야 상실 범위를 표시한다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Debug")
	bool bDrawDebugSight = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|AI")
	TObjectPtr<UBlackboardData> BlackboardAsset;

	/** 지정하지 않으면 생성자의 기본 시야 설정을 사용한다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Perception")
	TObjectPtr<UGAISightDataAsset> SightDataAsset;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Perception")
	TObjectPtr<AActor> SensedPlayer;

	/** 발견/상실 연출을 Blueprint에서 연결하기 위한 이벤트다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Guest|GuardDog|Perception")
	void OnPlayerSensed(AActor* PlayerActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Guest|GuardDog|Perception")
	void OnPlayerLost(AActor* PlayerActor);

private:
	void ApplySightSettings();

	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
