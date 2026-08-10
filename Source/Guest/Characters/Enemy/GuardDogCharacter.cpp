// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Characters/Enemy/GuardDogCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Guest/AI/Controllers/GuardDogAIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Guest/Characters/Player/GuestCharacter.h"

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
	FHitResult AttackHit;
	const bool bHitPlayer = PerformAttackTrace(AttackHit);

	OnAttackRequested();

	if (!bHitPlayer)
	{
		return;
	}
}

bool AGuardDogCharacter::PerformAttackTrace(FHitResult& OutHit) const
{
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = TraceStart + GetActorForwardVector() * AttackRange;
	
	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;

	if (bDrawDebugAttackTrace)
	{
		DebugType = EDrawDebugTrace::ForDuration;
	}
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(const_cast<AGuardDogCharacter*>(this));
	
	TArray<FHitResult> HitResults;
	
	UKismetSystemLibrary::SphereTraceMulti(
		this,
		TraceStart,
		TraceEnd,
		AttackRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		ActorsToIgnore,
		DebugType,
		HitResults,
		true
	);

	for (const FHitResult& Hit : HitResults)
	{
		if (AGuestCharacter* Player = Cast<AGuestCharacter>(Hit.GetActor()))
		{
			OutHit = Hit;
			return true;
		}
	}

	return false;
}
