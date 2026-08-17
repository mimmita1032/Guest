// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Animation/GuardDogAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Guest/Characters/Enemy/GuardDogCharacter.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"

void UGuardDogAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CacheGuardDogCharacter();
}

void UGuardDogAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!GuardDogCharacter)
	{
		CacheGuardDogCharacter();
	}

	UpdateMovementValues();
	UpdateGuardDogState();
}

void UGuardDogAnimInstance::SetAttacking(const bool bNewAttacking)
{
	bIsAttacking = bNewAttacking;
}

void UGuardDogAnimInstance::CacheGuardDogCharacter()
{
	GuardDogCharacter = Cast<AGuardDogCharacter>(TryGetPawnOwner());
}

void UGuardDogAnimInstance::UpdateMovementValues()
{
	if (!GuardDogCharacter)
	{
		GroundSpeed = 0.f;
		bIsMoving = false;
		bIsFalling = false;
		return;
	}

	const FVector Velocity = GuardDogCharacter->GetVelocity();
	const FVector HorizontalVelocity(Velocity.X, Velocity.Y, 0.f);

	GroundSpeed = HorizontalVelocity.Size();
	bIsMoving = GroundSpeed > 3.f;

	if (const UCharacterMovementComponent* MovementComponent = GuardDogCharacter->GetCharacterMovement())
	{
		bIsFalling = MovementComponent->IsFalling();
	}
	else
	{
		bIsFalling = false;
	}
}

void UGuardDogAnimInstance::UpdateGuardDogState()
{
	if (!GuardDogCharacter)
	{
		bIsChasing = false;
		bIsDead = false;
		return;
	}

	bIsChasing = GuardDogCharacter->IsChasing();

	const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(GuardDogCharacter);
	const UAbilitySystemComponent* AbilitySystemComponent =
		AbilitySystemInterface ? AbilitySystemInterface->GetAbilitySystemComponent() : nullptr;

	bIsDead = AbilitySystemComponent &&
		AbilitySystemComponent->HasMatchingGameplayTag(GuestGameplayTags::TAG_State_Dead);
}
