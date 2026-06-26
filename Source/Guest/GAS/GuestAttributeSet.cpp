// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"

void UGuestAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float IncomingDamage = GetDamage();
		SetDamage(0.f);

		if (IncomingDamage > 0.f)
		{
			SetCurrentHealth(FMath::Clamp(GetCurrentHealth() - IncomingDamage, 0.f, GetMaxHealth()));

			if (GetCurrentHealth() <= 0.f)
			{
				ApplyDeath();
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth()));

		if (GetCurrentHealth() <= 0.f)
		{
			ApplyDeath();
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
	else if (Data.EvaluatedData.Attribute == GetPurifyAmountAttribute())
	{
		const float IncomingPurify = GetPurifyAmount();
		SetPurifyAmount(0.f);

		if (IncomingPurify > 0.f)
		{
			SetPurifyGauge(FMath::Clamp(GetPurifyGauge() + IncomingPurify, 0.f, GetMaxPurifyGauge()));

			if (GetPurifyGauge() >= GetMaxPurifyGauge())
			{
				ApplyDeath();
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetPurifyGaugeAttribute())
	{
		SetPurifyGauge(FMath::Clamp(GetPurifyGauge(), 0.f, GetMaxPurifyGauge()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxPurifyGaugeAttribute())
	{
		SetMaxPurifyGauge(FMath::Max(0.f, GetMaxPurifyGauge()));
		SetPurifyGauge(FMath::Clamp(GetPurifyGauge(), 0.f, GetMaxPurifyGauge()));
	}
}

void UGuestAttributeSet::ApplyDeath()
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC) return;

	if (!ASC->HasMatchingGameplayTag(GuestGameplayTags::TAG_State_Dead))
	{
		ASC->AddLooseGameplayTag(GuestGameplayTags::TAG_State_Dead);
	}
}