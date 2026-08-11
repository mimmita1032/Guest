// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Characters/Enemy/GuardDogCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Guest/AI/Controllers/GuardDogAIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"

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

bool AGuardDogCharacter::RequestAttack()
{
	if (!CanAttack())
	{
		return false;
	}

	MarkAttackRequested();

	FHitResult AttackHit;
	const bool bHitPlayer = PerformAttackTrace(AttackHit);

	OnAttackRequested();

	if (!bHitPlayer)
	{
		return false;
	}

	return ApplyAttackDamage(AttackHit.GetActor());
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

bool AGuardDogCharacter::ApplyAttackDamage(AActor* TargetActor) const
{
	if (!TargetActor || !DamageEffectClass)
	{
		return false;
	}

	const IAbilitySystemInterface* TargetASCInterface = Cast<IAbilitySystemInterface>(TargetActor);
	if (!TargetASCInterface)
	{
		return false;
	}

	UAbilitySystemComponent* TargetASC = TargetASCInterface->GetAbilitySystemComponent();
	if (!TargetASC)
	{
		return false;
	}

	const IAbilitySystemInterface* SourceASCInterface = Cast<IAbilitySystemInterface>(this);
	if (!SourceASCInterface)
	{
		return false;
	}

	UAbilitySystemComponent* SourceASC = SourceASCInterface->GetAbilitySystemComponent();
	if (!SourceASC)
	{
		return false;
	}

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffectClass,
		1.f,
		SourceASC->MakeEffectContext()
	);

	if (!SpecHandle.IsValid())
	{
		return false;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(
		GuestGameplayTags::TAG_Data_DamageAmount,
		-AttackDamage
	);

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);
	return true;
}

bool AGuardDogCharacter::CanAttack() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const float CurrentTime = World->GetTimeSeconds();
	return CurrentTime - LastAttackTime >= AttackCooldown;
}

void AGuardDogCharacter::MarkAttackRequested()
{
	if (const UWorld* World = GetWorld())
	{
		LastAttackTime = World->GetTimeSeconds();
	}
}
