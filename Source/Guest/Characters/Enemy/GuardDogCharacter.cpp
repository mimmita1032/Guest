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

	// Blueprint에서 변경한 이동속도를 게임 시작 시 최종 반영한다.
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

	// 추격 여부에 따라 CharacterMovement의 최대 이동속도를 교체한다.
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
