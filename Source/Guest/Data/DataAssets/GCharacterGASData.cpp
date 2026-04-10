// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GCharacterGASData.h"
#include "Guest/GAS/GuestAbilitySystemComponent.h"

void UGCharacterGASData::GiveToASC(UGuestAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	checkf(InASCToGive, TEXT("ASC 추출 불가"));
	
	GiveEffectsToASC(StartUpEffects, InASCToGive, ApplyLevel);
}

void UGCharacterGASData::GiveEffectsToASC(const TArray<TSubclassOf<UGameplayEffect>> EffectsToGive,
                                     UGuestAbilitySystemComponent* InASC, int32 ApplyLevel)
{
	if (!EffectsToGive.IsEmpty())
	{
		for(const TSubclassOf<UGameplayEffect>& EffectClass: EffectsToGive)
		{
			if (!EffectClass) continue;

			UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();

			InASC->ApplyGameplayEffectToSelf(
				EffectCDO,
				ApplyLevel,
				InASC->MakeEffectContext()
				);
		}
	}
}
