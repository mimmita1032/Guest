// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Guest/GuestTypes/GuestGameplayTypes.h"
#include "GSkillMasterySubsystem.generated.h"

class UGSkillDefinition;

// Discover 성공 또는 Mastered 전환 시 Broadcast → UI/SaveGame 등이 구독
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillStateChanged, FGameplayTag, SkillTag, ESkillState, NewState);

// Progress가 실제로 증가했을 때만 Broadcast (변화 없으면 발생하지 않음)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSkillProgressChanged, FGameplayTag, SkillTag, FGameplayTag, ProgressEventTag, float, NewProgress);

/**
 * 전문성(Skill) Definition 비동기 로드/캐시와 플레이어 Runtime 상태(Locked/InTheory/Mastered)를 관리한다.
 * Quest 시스템과 동일하게 UGameInstanceSubsystem 수명주기를 따르되 서로 직접 의존하지 않는다.
 */
UCLASS()
class GUEST_API UGSkillMasterySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

#pragma region Delegate
public:
	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnSkillStateChanged OnSkillStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnSkillProgressChanged OnSkillProgressChanged;
#pragma endregion

#pragma region Definition Query
public:
	// Skill Definition 로드 및 캐시 구축이 끝났는지 여부
	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool IsSkillDataReady() const { return bSkillDataReady; }

	// 캐시 기반 O(1) 조회. 데이터 미로드/미존재 SkillTag는 nullptr.
	const UGSkillDefinition* FindSkillDefinition(FGameplayTag SkillTag) const;
#pragma endregion

#pragma region Runtime State Query
public:
	// 잘못된 SkillTag/데이터 미로드 시 Locked를 안전한 기본값으로 반환
	UFUNCTION(BlueprintCallable, Category = "Skill")
	ESkillState GetSkillState(FGameplayTag SkillTag) const;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool IsSkillMastered(FGameplayTag SkillTag) const;

	// 잘못된 SkillTag/ProgressEventTag/데이터 미로드 시 0.f 반환
	UFUNCTION(BlueprintCallable, Category = "Skill")
	float GetSkillProgress(FGameplayTag SkillTag, FGameplayTag ProgressEventTag) const;
#pragma endregion

#pragma region Discovery & Progress
public:
	// Book 연동은 Phase 3. 여기서는 Locked → InTheory 전이만 담당하는 Runtime API.
	// 반환값 true = 실제로 InTheory로 전이됨, false = Data 미준비 / 존재하지 않는 SkillTag / 이미 Locked가 아님
	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool DiscoverSkill(FGameplayTag SkillTag);

	// Phase 3의 실제 이벤트 발신부(Flash 사용, WeakPoint 관찰 등)가 호출할 진행도 누적 API
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void HandleSkillProgressEvent(FGameplayTag ProgressEventTag, float Amount);
#pragma endregion

	/*===========================================================
	 * [디버그 전용] Skill 시스템 콘솔/BP 테스트 함수
	 * 실제 발신부(Phase 3)가 준비되기 전까지 PIE에서 Discover/Progress 흐름을 검증하기 위한 용도.
	 * DiscoverSkill/HandleSkillProgressEvent를 그대로 호출하는 얇은 래퍼이며 별도 상태를 갖지 않는다.
	 *===========================================================*/
#pragma region Debug
public:
	UFUNCTION(BlueprintCallable, Category = "Skill|Debug")
	bool Debug_DiscoverSkill(FGameplayTag SkillTag) { return DiscoverSkill(SkillTag); }

	UFUNCTION(BlueprintCallable, Category = "Skill|Debug")
	void Debug_AddSkillProgress(FGameplayTag ProgressEventTag, float Amount) { HandleSkillProgressEvent(ProgressEventTag, Amount); }
#pragma endregion

private:
	// LoadGuestSkills() 완료 Callback. 여기서만 캐시를 구축한다.
	void OnSkillAssetsLoaded();

	// Progress가 실제로 바뀐 Skill 하나에 대해서만 Mastery 조건을 재검사한다 (전체 재검사 아님).
	void TryMasterSkill(FGameplayTag SkillTag);

	// RequiredSkills가 모두 Mastered인지 확인. Invalid/존재하지 않는 선행 Skill은 로그 후 미충족으로 처리.
	bool ArePrerequisiteSkillsMastered(const UGSkillDefinition& Definition) const;

private:
	// AssetManager가 내부적으로 PrimaryAsset을 강하게 참조하지만, Subsystem 스스로도 독립적인 GC 루트를
	// 갖도록 UPROPERTY + TObjectPtr로 보관한다 (GuestUISubsystem::StackMap과 동일한 관례).
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UGSkillDefinition>> SkillDefinitionCache;

	// UObject를 담지 않는 순수 데이터라 UPROPERTY 없이도 GC 이슈가 없다.
	TMap<FGameplayTag, FSkillRuntimeState> SkillStates;

	bool bSkillDataReady = false;
};