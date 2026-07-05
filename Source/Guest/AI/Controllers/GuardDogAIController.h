// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GuardDogAIController.generated.h"

class UGAISightDataAsset;
class UBlackboardData;

/**
 * Controller dedicated to hostile guard dogs.
 * Perception and behavior-tree setup will be added in the following commits.
 */
UCLASS()
class GUEST_API AGuardDogAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGuardDogAIController();

	static const FName BB_TargetActor;
	static const FName BB_LastKnownLocation;
	static const FName BB_IsAlerted;

	UFUNCTION(BlueprintPure, Category = "Guest|GuardDog|Perception")
	AActor* GetSensedPlayer() const { return SensedPlayer; }

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|AI")
	TObjectPtr<UBlackboardData> BlackboardAsset;

	/** Optional tuning asset. SightConfig fallback values are used when unset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Perception")
	TObjectPtr<UGAISightDataAsset> SightDataAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Guest|GuardDog|Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Guest|GuardDog|Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Perception")
	TObjectPtr<AActor> SensedPlayer;

	UFUNCTION(BlueprintImplementableEvent, Category = "Guest|GuardDog|Perception")
	void OnPlayerSensed(AActor* PlayerActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Guest|GuardDog|Perception")
	void OnPlayerLost(AActor* PlayerActor);

private:
	void ApplySightSettings();

	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
