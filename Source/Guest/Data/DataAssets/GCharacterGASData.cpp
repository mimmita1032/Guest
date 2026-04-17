// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GCharacterGASData.h"
#include "Guest/GAS/GuestAbilitySystemComponent.h"
#include "Guest/GAS/GuestAttributeSet.h"
#include "Guest/GuestTypes/GuestGameplayTypes.h"

void UGCharacterGASData::GiveToASC(UGuestAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	checkf(InASCToGive, TEXT("ASC 추출 불가"));
	
	InitBaseStatsToASC(InASCToGive, CharacterBaseStatDataTable, ApplyLevel);
	GiveEffectsToASC(InitialEffects, InASCToGive, ApplyLevel);
}

void UGCharacterGASData::InitBaseStatsToASC(UGuestAbilitySystemComponent* InASCToGive, UDataTable* BaseStatTableToGive,
	int32 ApplyLevel)
{	
	if (!CharacterBaseStatDataTable) return;
	
	const FGuestCharacterBaseStats* BaseStats = nullptr;
	
	for (const TPair<FName, uint8*>& DataPair : CharacterBaseStatDataTable->GetRowMap())
	{
		const FGuestCharacterBaseStats* Row = reinterpret_cast<const FGuestCharacterBaseStats*>(DataPair.Value);
		
		if (!Row) continue;
		
		if (Row->Class == InASCToGive->GetOwner()->GetClass())
		{
			BaseStats = Row;
			break;
		}
	}
	if (BaseStats)
	{
		InASCToGive->SetNumericAttributeBase(UGuestAttributeSet::GetMaxHealthAttribute(), BaseStats->Health);
		InASCToGive->SetNumericAttributeBase(UGuestAttributeSet::GetMaxBatteryAttribute(), BaseStats->Battery);
	}
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
