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
 * 타겟에게 GE_Damage를 적용한다. DamageAmount는 양수로 전달하며,
 * 체력 감소는 AttributeSet::PostGameplayEffectExecute 가 담당한다.
 *
 * [에디터 설정]
 * DamageEffectClass : GE_Damage Blueprint 할당 (Damage 메타 속성 대상)
 * DamageAmount      : 타격당 데미지 수치 (양수)
 * AttackRange       : 전방 판정 거리 (cm)
 * AttackRadius      : 판정 구 반경 (cm)
 * bShowDebugTrace   : 개발 빌드에서 트레이스 시각화 여부
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

	// Shipping 빌드에서는 항상 비활성화. 개발 빌드에서만 유효.
	UPROPERTY(EditDefaultsOnly, Category = "Melee|Debug")
	bool bShowDebugTrace = true;

private:
	void PerformMeleeTrace(AActor* AvatarActor, TArray<FHitResult>& OutHitResults) const;
	void ApplyDamageToTarget(AActor* TargetActor, UAbilitySystemComponent* SourceASC) const;
};