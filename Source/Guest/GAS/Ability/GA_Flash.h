// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GuestGameplayAbility.h"
#include "GA_Flash.generated.h"

/**
 * UGA_Flash
 *
 * 플래시 어빌리티. GA_Melee와 동일한 SphereTrace로 전방 타겟을 탐색한 뒤
 * 타겟의 EnemyIdentityTag에 따라 두 가지 GE를 분기 적용한다.
 *
 *   방해꾼 (TAG_Enemy_Type_Reality)   → GE_Blinded (Duration, State.Blinded 부여)
 *   잔상   (TAG_Enemy_Type_AfterImage) → GE_Purify  (Instant, PurifyAmount SetByCaller)
 *
 * [에디터 설정]
 * BlindedEffectClass  : GE_Blinded Blueprint 할당
 * PurifyEffectClass   : GE_Purify Blueprint 할당
 * PurifyAmount        : 타격당 정화량 (양수)
 * FlashRange          : 전방 판정 거리 (cm)
 * FlashRadius         : 판정 구 반경 (cm)
 * bShowDebugTrace     : 개발 빌드에서 트레이스 시각화 여부
 */
UCLASS()
class GUEST_API UGA_Flash : public UGuestGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Flash();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Flash")
	TSubclassOf<UGameplayEffect> BlindedEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Flash")
	TSubclassOf<UGameplayEffect> PurifyEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Flash")
	float PurifyAmount = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Flash")
	float FlashRange = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Flash")
	float FlashRadius = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Flash|Debug")
	bool bShowDebugTrace = true;

private:
	void PerformFlashTrace(AActor* AvatarActor, TArray<FHitResult>& OutHitResults) const;
	void ApplyBlindedToTarget(AActor* TargetActor, UAbilitySystemComponent* SourceASC) const;
	void ApplyPurifyToTarget(AActor* TargetActor, UAbilitySystemComponent* SourceASC) const;
};