// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Characters/Enemy/GuardDogCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Guest/AI/Controllers/GuardDogAIController.h"

AGuardDogCharacter::AGuardDogCharacter()
{
	AIControllerClass = AGuardDogAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = WalkSpeed;
		Movement->bOrientRotationToMovement = true;
	}

	bUseControllerRotationYaw = false;
}

void AGuardDogCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Apply Blueprint-overridden defaults after the class default object is initialized.
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = bIsChasing ? ChaseSpeed : WalkSpeed;
	}
}

void AGuardDogCharacter::SetChasing(const bool bNewChasing)
{
	if (bIsChasing == bNewChasing)
	{
		return;
	}

	bIsChasing = bNewChasing;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = bIsChasing ? ChaseSpeed : WalkSpeed;
	}

	OnChasingStateChanged(bIsChasing);
}

void AGuardDogCharacter::RequestAttack()
{
	OnAttackRequested();
}
