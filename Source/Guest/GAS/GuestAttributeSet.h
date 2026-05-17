// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GuestAbilitySystemComponent.h"
#include "GuestAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GUEST_API UGuestAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	ATTRIBUTE_ACCESSORS(UGuestAttributeSet, CurrentHealth)
	ATTRIBUTE_ACCESSORS(UGuestAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UGuestAttributeSet, CurrentBattery)
	ATTRIBUTE_ACCESSORS(UGuestAttributeSet, MaxBattery)

private:
	UPROPERTY()
	FGameplayAttributeData CurrentHealth;

	UPROPERTY()
	FGameplayAttributeData MaxHealth;

	UPROPERTY()
	FGameplayAttributeData CurrentBattery;

	UPROPERTY()
	FGameplayAttributeData MaxBattery;

};
