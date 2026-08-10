// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/Characters/Enemy/GuestEnemyCharacter.h"
#include "GuardDogCharacter.generated.h"

class UGameplayEffect;

/**
 * 맹견 몹의 캐릭터 베이스.
 * 이동 상태와 Blueprint 연동 지점을 담당한다.
 */
UCLASS(Blueprintable)
class GUEST_API AGuardDogCharacter : public AGuestEnemyCharacter
{
	GENERATED_BODY()

public:
	AGuardDogCharacter();
	virtual void BeginPlay() override;

	/** 일반 이동과 추격 이동속도를 전환한다. */
	UFUNCTION(BlueprintCallable, Category = "Guest|GuardDog|Movement")
	void SetChasing(bool bNewChasing);

	UFUNCTION(BlueprintPure, Category = "Guest|GuardDog|Movement")
	bool IsChasing() const { return bIsChasing; }

	/** 추후 BT Task 또는 Gameplay Ability가 호출할 공격 진입점이다. */
	UFUNCTION(BlueprintCallable, Category = "Guest|GuardDog|Combat")
	void RequestAttack();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Movement", meta = (ClampMin = "0.0", Units = "cm/s"))
	float WalkSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Movement", meta = (ClampMin = "0.0", Units = "cm/s"))
	float ChaseSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Combat", meta = (ClampMin = "0.0", Units = "cm"))
	float AttackRange = 180.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Combat", meta = (ClampMin = "0.0", Units = "cm"))
	float AttackRadius = 55.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Combat", meta = (ClampMin = "0.0", Units = "s"))
	float AttackCooldown = 1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Combat", meta = (ClampMin = "0.0"))
	float AttackDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Combat|Debug")
	bool bDrawDebugAttackTrace = true;

	/** 추격 상태 변경을 Animation Blueprint 등에 전달한다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Guest|GuardDog|Movement")
	void OnChasingStateChanged(bool bNowChasing);

	/** 실제 피해와 쿨다운 구현 전 사용할 Blueprint 공격 이벤트다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Guest|GuardDog|Combat")
	void OnAttackRequested();
	
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Guest|GuardDog|Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsChasing = false;
	
	bool PerformAttackTrace(FHitResult& OutHit) const;
};
