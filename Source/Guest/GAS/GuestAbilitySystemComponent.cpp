// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GuestAbilitySystemComponent.h"


void UGuestAbilitySystemComponent::OnAbilityPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;
		
		TryActivateAbility(AbilitySpec.Handle);
	}
}

void UGuestAbilitySystemComponent::OnAbilityReleased(const FGameplayTag& InputTag)
{
	// 키 홀드 ability cancel 처리
	
	UE_LOG(LogTemp, Warning, TEXT("어빌리티 인풋키 업"));
}
