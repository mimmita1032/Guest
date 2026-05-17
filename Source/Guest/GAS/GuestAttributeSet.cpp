// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"

void UGuestAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth()));

		if (GetCurrentHealth() <= 0.f)
		{
			UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
			if (ASC && !ASC->HasMatchingGameplayTag(GuestGameplayTags::TAG_State_Dead))
			{
				ASC->AddLooseGameplayTag(GuestGameplayTags::TAG_State_Dead);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Max(0.f, GetMaxHealth()));
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetCurrentBatteryAttribute())
	{
		SetCurrentBattery(FMath::Clamp(GetCurrentBattery(), 0.f, GetMaxBattery()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxBatteryAttribute())
	{
		SetMaxBattery(FMath::Max(0.f, GetMaxBattery()));
		SetCurrentBattery(FMath::Clamp(GetCurrentBattery(), 0.f, GetMaxBattery()));
	}
}
