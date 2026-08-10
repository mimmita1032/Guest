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

	// 잔상(AfterImage) Enemy 전용. 0이면 정화 시스템 미사용.
	UPROPERTY(EditAnywhere)
	float MaxPurifyGauge = 0.f;
};

USTRUCT(BlueprintType)
struct FGuestAbilitySet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Guest.InputTag"))
	FGameplayTag InputTag;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGuestGameplayAbility> AbilityToGrant;
	
};