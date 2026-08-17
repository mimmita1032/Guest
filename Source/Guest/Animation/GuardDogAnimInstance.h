// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GuardDogAnimInstance.generated.h"

class AGuardDogCharacter;

UCLASS(Blueprintable, BlueprintType)
class GUEST_API UGuardDogAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Guest|GuardDog|Animation")
	void SetAttacking(bool bNewAttacking);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Guest|GuardDog|Animation")
	TObjectPtr<AGuardDogCharacter> GuardDogCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Guest|GuardDog|Animation")
	float GroundSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Guest|GuardDog|Animation")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "Guest|GuardDog|Animation")
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, Category = "Guest|GuardDog|Animation")
	bool bIsChasing = false;

	UPROPERTY(BlueprintReadWrite, Category = "Guest|GuardDog|Animation")
	bool bIsAttacking = false;

	UPROPERTY(BlueprintReadOnly, Category = "Guest|GuardDog|Animation")
	bool bIsDead = false;

private:
	void CacheGuardDogCharacter();
	void UpdateMovementValues();
	void UpdateGuardDogState();
};
