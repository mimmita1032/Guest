// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GA_Melee.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"

UGA_Melee::UGA_Melee()
{
	FGameplayTagContainer Tags;
	Tags.AddTag(GuestGameplayTags::TAG_Ability_Melee);
	SetAssetTags(Tags);

	BlockAbilitiesWithTag.AddTag(GuestGameplayTags::TAG_Ability_Melee);
}

void UGA_Melee::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();

	if (!AvatarActor || !SourceASC || !DamageEffectClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TArray<FHitResult> HitResults;
	PerformMeleeTrace(AvatarActor, HitResults);

	for (const FHitResult& Hit : HitResults)
	{
		ApplyDamageToTarget(Hit.GetActor(), SourceASC);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UGA_Melee::PerformMeleeTrace(AActor* AvatarActor, TArray<FHitResult>& OutHitResults) const
{
	const FVector TraceStart = AvatarActor->GetActorLocation();
	const FVector TraceEnd = TraceStart + AvatarActor->GetActorForwardVector() * AttackRange;

#if UE_BUILD_SHIPPING
	constexpr EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
#else
	const EDrawDebugTrace::Type DebugType = bShowDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
#endif

	TArray<AActor*> ActorsToIgnore = { AvatarActor };

	UKismetSystemLibrary::SphereTraceMulti(
		AvatarActor,
		TraceStart,
		TraceEnd,
		AttackRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		ActorsToIgnore,
		DebugType,
		OutHitResults,
		true
	);
}

void UGA_Melee::ApplyDamageToTarget(AActor* TargetActor, UAbilitySystemComponent* SourceASC) const
{
	if (!TargetActor || !SourceASC) return;

	const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetActor);
	if (!ASCInterface) return;

	UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
	if (!TargetASC) return;

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());

	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(GuestGameplayTags::TAG_Data_Damage, DamageAmount);
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);
}