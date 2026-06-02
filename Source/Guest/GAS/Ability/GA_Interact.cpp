// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GA_Interact.h"
#include "Guest/Components/Interaction/GInteractionComponent.h"
#include "Guest/Utils/GLog.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"

UGA_Interact::UGA_Interact()
{
	AbilityTags.AddTag(GuestGameplayTags::TAG_Ability_Interact);
	BlockAbilitiesWithTag.AddTag(GuestGameplayTags::TAG_Ability_Interact); // GA 활성화중 중복 사용 방지
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) //코스트, 쿨다운 체크
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,false, false);
		return;
	}
	
	UGInteractionComponent* InteractionComponent = nullptr;
	
	InteractionComponent = GetAvatarActorFromActorInfo()->FindComponentByClass<UGInteractionComponent>();
	
	if (InteractionComponent)
	{
		G_LOG(TEXT("상호작용 키 입력됨"));
		InteractionComponent->DoInteract();
	}
	
	//Todo :: 애니메이션 재생 등
	
	EndAbility(Handle,ActorInfo,ActivationInfo,false, false);
	return;
}
