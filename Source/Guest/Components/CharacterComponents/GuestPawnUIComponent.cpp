// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestPawnUIComponent.h"
#include "AbilitySystemComponent.h"
#include "Guest/GAS/GuestAttributeSet.h"

UGuestPawnUIComponent::UGuestPawnUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGuestPawnUIComponent::InitializeWithASC(UAbilitySystemComponent* InASC)
{
	if (!InASC) return;

	CachedASC = InASC;

	HealthChangedHandle = InASC->GetGameplayAttributeValueChangeDelegate(
		UGuestAttributeSet::GetCurrentHealthAttribute())
		.AddUObject(this, &UGuestPawnUIComponent::HandleHealthChanged);

	BatteryChangedHandle = InASC->GetGameplayAttributeValueChangeDelegate(
		UGuestAttributeSet::GetCurrentBatteryAttribute())
		.AddUObject(this, &UGuestPawnUIComponent::HandleBatteryChanged);
}

void UGuestPawnUIComponent::RequestCurrentValues()
{
	if (!CachedASC.IsValid()) return;

	const UGuestAttributeSet* AS = CachedASC->GetSet<UGuestAttributeSet>();
	if (!AS) return;

	OnHealthChanged.Broadcast(AS->GetCurrentHealth(), AS->GetMaxHealth());
	OnBatteryChanged.Broadcast(AS->GetCurrentBattery(), AS->GetMaxBattery());
}

void UGuestPawnUIComponent::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!CachedASC.IsValid()) return;

	const UGuestAttributeSet* AS = CachedASC->GetSet<UGuestAttributeSet>();
	if (!AS) return;

	OnHealthChanged.Broadcast(Data.NewValue, AS->GetMaxHealth());
}

void UGuestPawnUIComponent::HandleBatteryChanged(const FOnAttributeChangeData& Data)
{
	if (!CachedASC.IsValid()) return;

	const UGuestAttributeSet* AS = CachedASC->GetSet<UGuestAttributeSet>();
	if (!AS) return;

	OnBatteryChanged.Broadcast(Data.NewValue, AS->GetMaxBattery());
}
