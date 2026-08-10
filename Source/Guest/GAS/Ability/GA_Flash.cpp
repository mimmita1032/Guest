// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GA_Flash.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"

UGA_Flash::UGA_Flash()
{
	FGameplayTagContainer Tags;
	Tags.AddTag(GuestGameplayTags::TAG_Ability_Flash);
	SetAssetTags(Tags);

	BlockAbilitiesWithTag.AddTag(GuestGameplayTags::TAG_Ability_Flash);
}

void UGA_Flash::ActivateAbility(
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

	if (!AvatarActor || !SourceASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TArray<FHitResult> HitResults;
	PerformFlashTrace(AvatarActor, HitResults);

	TArray<AActor*> AlreadyHit;
	for (const FHitResult& Hit : HitResults)
	{
		AActor* TargetActor = Hit.GetActor();
		if (!TargetActor || AlreadyHit.Contains(TargetActor)) continue;
		AlreadyHit.Add(TargetActor);

		const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetActor);
		if (!ASCInterface) continue;

		UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
		if (!TargetASC) continue;

		if (TargetASC->HasMatchingGameplayTag(GuestGameplayTags::TAG_Enemy_Type_AfterImage))
		{
			ApplyPurifyToTarget(TargetActor, SourceASC);
		}
		else if (TargetASC->HasMatchingGameplayTag(GuestGameplayTags::TAG_Enemy_Type_Reality))
		{
			ApplyBlindedToTarget(TargetActor, SourceASC);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UGA_Flash::PerformFlashTrace(AActor* AvatarActor, TArray<FHitResult>& OutHitResults) const
{
	const FVector TraceStart = AvatarActor->GetActorLocation();
	const FVector TraceEnd = TraceStart + AvatarActor->GetActorForwardVector() * FlashRange;

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
		FlashRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		ActorsToIgnore,
		DebugType,
		OutHitResults,
		true
	);
}

void UGA_Flash::ApplyBlindedToTarget(AActor* TargetActor, UAbilitySystemComponent* SourceASC) const
{
	if (!TargetActor || !SourceASC || !BlindedEffectClass) return;

	const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetActor);
	if (!ASCInterface) return;

	UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
	if (!TargetASC) return;

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		BlindedEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());

	if (!SpecHandle.IsValid()) return;

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);
}

void UGA_Flash::ApplyPurifyToTarget(AActor* TargetActor, UAbilitySystemComponent* SourceASC) const
{
	if (!TargetActor || !SourceASC || !PurifyEffectClass) return;

	const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetActor);
	if (!ASCInterface) return;

	UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
	if (!TargetASC) return;

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		PurifyEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());

	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(GuestGameplayTags::TAG_Data_PurifyAmount, PurifyAmount);
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);
}