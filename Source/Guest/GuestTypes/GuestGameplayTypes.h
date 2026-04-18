// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "GuestGameplayTypes.Generated.h"

class UGuestGameplayAbility;

USTRUCT( BlueprintType )
struct FGuestCharacterBaseStats : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;
	
	UPROPERTY(EditAnywhere)
	float Health;
	
	UPROPERTY(EditAnywhere)
	float Battery;
};

USTRUCT(BlueprintType)
struct FGuestAbilitySet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Input"))
	FGameplayTag InputTag;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGuestGameplayAbility> AbilityToGrant;
	
};