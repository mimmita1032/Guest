// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/Characters/Enemy/GuestEnemyCharacter.h"
#include "GuardDogCharacter.generated.h"

/**
 * Hostile dog character base.
 * Detection, chase decisions, and attacks are intentionally left to later commits.
 */
UCLASS(Blueprintable)
class GUEST_API AGuardDogCharacter : public AGuestEnemyCharacter
{
	GENERATED_BODY()

public:
	AGuardDogCharacter();
	virtual void BeginPlay() override;

	/** Switches between the normal and chase movement speeds. */
	UFUNCTION(BlueprintCallable, Category = "Guest|GuardDog|Movement")
	void SetChasing(bool bNewChasing);

	UFUNCTION(BlueprintPure, Category = "Guest|GuardDog|Movement")
	bool IsChasing() const { return bIsChasing; }

	/** Entry point for a future attack task or gameplay ability. */
	UFUNCTION(BlueprintCallable, Category = "Guest|GuardDog|Combat")
	void RequestAttack();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Movement", meta = (ClampMin = "0.0", Units = "cm/s"))
	float WalkSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Movement", meta = (ClampMin = "0.0", Units = "cm/s"))
	float ChaseSpeed = 600.f;

	/** Animation Blueprints can react to chase state changes through this hook. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Guest|GuardDog|Movement")
	void OnChasingStateChanged(bool bNowChasing);

	/** Blueprint hook only; damage and cooldown logic will be implemented later. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Guest|GuardDog|Combat")
	void OnAttackRequested();

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsChasing = false;
};
