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

	const FVector TraceStart = AvatarActor->GetActorLocation();
	const FVector TraceEnd   = TraceStart + AvatarActor->GetActorForwardVector() * AttackRange;

	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore = { AvatarActor };

	UKismetSystemLibrary::SphereTraceMulti(
		AvatarActor,
		TraceStart,
		TraceEnd,
		AttackRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResults,
		true
	);

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;

		IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(HitActor);
		if (!ASCInterface) continue;

		UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
		if (!TargetASC) continue;

		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
			DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());

		if (!SpecHandle.IsValid()) continue;

		SpecHandle.Data->SetSetByCallerMagnitude(GuestGameplayTags::TAG_Data_Damage, -DamageAmount);
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
