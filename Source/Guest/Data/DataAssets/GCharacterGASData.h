// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GCharacterGASData.generated.h"

/**
 * 
 */
class UGameplayEffect;
class UGuestGameplayAbility;
class UGuestAbilitySystemComponent;
UCLASS()
class GUEST_API UGCharacterGASData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	virtual void GiveToASC(UGuestAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	UDataTable* CharacterBaseStatDataTable;
	
	UPROPERTY(EditDefaultsOnly,Category = "GASData")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	UPROPERTY(EditDefaultsOnly,Category = "GASData")
	TArray<TSubclassOf<UGameplayEffect>> AbilitiesByInputTag;
	
	void InitBaseStatsToASC(UGuestAbilitySystemComponent* InASCToGive,UDataTable* BaseStatTableToGive, int32 ApplyLevel);
	
	void GiveEffectsToASC(const TArray<TSubclassOf<UGameplayEffect>> EffectsToGive, UGuestAbilitySystemComponent* InASC, int32 ApplyLevel = 1);
};
