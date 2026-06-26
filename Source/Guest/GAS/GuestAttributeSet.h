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
	// 데미지 전달용 메타 속성. GE 적용 직후 0으로 리셋되며 저장되지 않는다.
	ATTRIBUTE_ACCESSORS(UGuestAttributeSet, Damage)

	ATTRIBUTE_ACCESSORS(UGuestAttributeSet, PurifyGauge)
	ATTRIBUTE_ACCESSORS(UGuestAttributeSet, MaxPurifyGauge)
	// 정화량 전달용 메타 속성. GE 적용 직후 0으로 리셋되며 저장되지 않는다.
	ATTRIBUTE_ACCESSORS(UGuestAttributeSet, PurifyAmount)

private:
	UPROPERTY()
	FGameplayAttributeData CurrentHealth;

	UPROPERTY()
	FGameplayAttributeData MaxHealth;

	UPROPERTY()
	FGameplayAttributeData CurrentBattery;

	UPROPERTY()
	FGameplayAttributeData MaxBattery;

	UPROPERTY()
	FGameplayAttributeData Damage;

	UPROPERTY()
	FGameplayAttributeData PurifyGauge;

	UPROPERTY()
	FGameplayAttributeData MaxPurifyGauge;

	UPROPERTY()
	FGameplayAttributeData PurifyAmount;

	void ApplyDeath();

};
