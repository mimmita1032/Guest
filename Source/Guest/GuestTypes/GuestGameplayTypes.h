// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "GuestGameplayTypes.Generated.h"

class UGuestGameplayAbility;
class UGameplayEffect;

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

// Skill Mastery 진행 조건. Count/Distance/Time 등 의미는 ProgressEventTag를 발신하는 쪽이 결정하고,
// Skill 시스템은 float 누적량만 처리한다.
USTRUCT(BlueprintType)
struct FSkillMasteryCondition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Guest.Skill.Progress"))
	FGameplayTag ProgressEventTag;

	UPROPERTY(EditDefaultsOnly)
	float RequiredAmount = 1.f;
};

USTRUCT(BlueprintType)
struct FSkillReward
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGuestGameplayAbility> GrantedAbility;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GrantedPassiveEffect;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag FeatureUnlockTag;
};

// 플레이어별 Skill 진행 상태. UGSkillDefinition(설계도)에는 저장하지 않는다.
UENUM(BlueprintType)
enum class ESkillState : uint8
{
	Locked,
	InTheory,
	Mastered
};

USTRUCT(BlueprintType)
struct FSkillRuntimeState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ESkillState State = ESkillState::Locked;

	// Key: FSkillMasteryCondition::ProgressEventTag, Value: 현재 누적량
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, float> ConditionProgress;
};