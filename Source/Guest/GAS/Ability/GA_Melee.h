// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GuestGameplayAbility.h"
#include "GA_Melee.generated.h"

/**
 * UGA_Melee
 *
 * 근접 공격 어빌리티.
 * SphereTrace로 전방 타겟을 탐색하고, IAbilitySystemInterface를 구현한
 * 타겟에게 GE_Damage를 적용한다.
 *
 * [에디터 설정]
 * DamageEffectClass : GE_Damage Blueprint 할당
 * DamageAmount      : 타격당 데미지 수치
 * AttackRange       : 전방 판정 거리 (cm)
 * AttackRadius      : 판정 구 반경 (cm)
 */
UCLASS()
class GUEST_API UGA_Melee : public UGuestGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Melee();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float DamageAmount = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float AttackRange = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float AttackRadius = 60.f;
};
