// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GuestGameplayAbility.h"
#include "GA_Interact.generated.h"

/**
 * 
 */
UCLASS()
class GUEST_API UGA_Interact : public UGuestGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Interact();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
