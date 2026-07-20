// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GuestEnemyCharacter.generated.h"

class UGuestAbilitySystemComponent;
class UGuestAttributeSet;
class UGCharacterGASData;

/**
 * AGuestEnemyCharacter
 *
 * 전투 Enemy 베이스 클래스.
 * ASC / AttributeSet을 소유하며 GE_Damage 피격과 사망 처리를 담당한다.
 * 나중에 AIController / BehaviorTree를 연결하여 확장 가능하도록 설계.
 *
 * [사망 처리 흐름]
 * GE_Damage 적용
 *   → AttributeSet::PostGameplayEffectExecute — HP 클램핑 + Dead Tag 추가
 *   → OnDeadTagChanged (ASC Tag Event) — HandleDeath 실행
 */
UCLASS()
class GUEST_API AGuestEnemyCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGuestEnemyCharacter();

	virtual void BeginPlay() override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// EnemyIdentityTag가 잔상(AfterImage) 타입인지 여부 — AGuestAfterimageAIController 등 외부에서 검증에 사용
	UFUNCTION(BlueprintPure, Category = "Enemy")
	bool IsAfterimageEnemy() const;

protected:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UGuestAbilitySystemComponent> GuestASC;

	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UGuestAttributeSet> GuestAttributeSet;

	// 에디터에서 Enemy 전용 DataAsset 할당
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSoftObjectPtr<UGCharacterGASData> EnemyGasData;

	// TAG_Enemy_Type_Reality 또는 TAG_Enemy_Type_AfterImage 중 하나를 할당
	UPROPERTY(EditDefaultsOnly, Category = "Enemy", meta = (Categories = "Guest.Enemy.Type"))
	FGameplayTag EnemyIdentityTag;

	virtual void HandleDeath();

private:
	UFUNCTION()
	void OnDeadTagChanged(const FGameplayTag Tag, int32 NewCount);

	// Blinded 해제 시 Reality Enemy의 경계 상태를 Calm으로 전환
	UFUNCTION()
	void OnBlindedTagChanged(const FGameplayTag Tag, int32 NewCount);
};
