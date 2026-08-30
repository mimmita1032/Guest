// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GSkillMasterySubsystem.h"

#include "Guest/Utils/GLog.h"
#include "Guest/FrameWork/GuestAssetManager.h"
#include "Guest/Data/DataAssets/GSkillDefinition.h"

void UGSkillMasterySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bSkillDataReady = false;

	UGuestAssetManager::Get().LoadGuestSkills(
		FStreamableDelegate::CreateUObject(this, &UGSkillMasterySubsystem::OnSkillAssetsLoaded));
}

void UGSkillMasterySubsystem::Deinitialize()
{
	SkillDefinitionCache.Reset();
	SkillStates.Reset();
	bSkillDataReady = false;

	Super::Deinitialize();
}

void UGSkillMasterySubsystem::OnSkillAssetsLoaded()
{
	TArray<const UGSkillDefinition*> LoadedSkills;
	UGuestAssetManager::Get().GetLoadedSkills(LoadedSkills);

	SkillDefinitionCache.Reset();
	SkillStates.Reset();

	for (const UGSkillDefinition* Definition : LoadedSkills)
	{
		if (!Definition)
		{
			continue;
		}

		if (!Definition->SkillTag.IsValid())
		{
			G_WARN(TEXT("Skill 시스템: [%s] SkillTag가 비어 있어 캐시에서 제외"), *Definition->GetName());
			continue;
		}

		if (SkillDefinitionCache.Contains(Definition->SkillTag))
		{
			G_ERR(TEXT("Skill 시스템: SkillTag [%s] 중복 Definition 발견 (%s) — 먼저 등록된 Definition을 유지하고 무시"),
				*Definition->SkillTag.ToString(), *Definition->GetName());
			continue;
		}

		// GetLoadedSkills()가 const 포인터로 반환하지만 실제로는 AssetManager가 소유한 비-const UObject다.
		// GItemDefinition 계열과 동일하게 constness는 조회부 계약일 뿐 실제 불변 객체가 아니다.
		SkillDefinitionCache.Add(Definition->SkillTag, const_cast<UGSkillDefinition*>(Definition));
		SkillStates.Add(Definition->SkillTag, FSkillRuntimeState());
	}

	bSkillDataReady = true;
	G_LOG(TEXT("Skill 시스템: Skill 데이터 로드 완료 (%d개 Definition 캐시)"), SkillDefinitionCache.Num());
}

const UGSkillDefinition* UGSkillMasterySubsystem::FindSkillDefinition(FGameplayTag SkillTag) const
{
	if (!bSkillDataReady)
	{
		return nullptr;
	}
	return SkillDefinitionCache.FindRef(SkillTag);
}

ESkillState UGSkillMasterySubsystem::GetSkillState(FGameplayTag SkillTag) const
{
	if (const FSkillRuntimeState* State = SkillStates.Find(SkillTag))
	{
		return State->State;
	}
	return ESkillState::Locked;
}

bool UGSkillMasterySubsystem::IsSkillMastered(FGameplayTag SkillTag) const
{
	return GetSkillState(SkillTag) == ESkillState::Mastered;
}

float UGSkillMasterySubsystem::GetSkillProgress(FGameplayTag SkillTag, FGameplayTag ProgressEventTag) const
{
	if (const FSkillRuntimeState* State = SkillStates.Find(SkillTag))
	{
		return State->ConditionProgress.FindRef(ProgressEventTag);
	}
	return 0.f;
}

bool UGSkillMasterySubsystem::DiscoverSkill(FGameplayTag SkillTag)
{
	if (!bSkillDataReady)
	{
		G_WARN(TEXT("Skill 시스템: 데이터 미준비 상태에서 DiscoverSkill(%s) 호출 무시"), *SkillTag.ToString());
		return false;
	}

	FSkillRuntimeState* State = SkillStates.Find(SkillTag);
	if (!State)
	{
		G_WARN(TEXT("Skill 시스템: 존재하지 않는 SkillTag [%s]로 DiscoverSkill 호출"), *SkillTag.ToString());
		return false;
	}

	if (State->State != ESkillState::Locked)
	{
		// 이미 InTheory 또는 Mastered — 중복 Discover로 상태를 되돌리거나 초기화하지 않는다.
		return false;
	}

	const UGSkillDefinition* Definition = SkillDefinitionCache.FindRef(SkillTag);
	if (!Definition)
	{
		G_WARN(TEXT("Skill 시스템: SkillTag [%s]의 Definition을 캐시에서 찾을 수 없어 DiscoverSkill 보류"), *SkillTag.ToString());
		return false;
	}

	// 정책: 선행 Skill이 모두 Mastered여야 책 내용을 이해할 수 있다.
	// 미충족 시 "아직 이해할 수 없는 내용입니다" — 상태를 바꾸지 않고 Locked를 유지한다.
	if (!ArePrerequisiteSkillsMastered(*Definition))
	{
		G_LOG(TEXT("Skill 시스템: [%s] 선행 Skill 미충족 — 아직 이해할 수 없는 내용입니다 (Discover 보류, Locked 유지)"),
			*SkillTag.ToString());
		return false;
	}

	State->State = ESkillState::InTheory;
	G_LOG(TEXT("Skill 시스템: [%s] Locked → InTheory"), *SkillTag.ToString());
	OnSkillStateChanged.Broadcast(SkillTag, ESkillState::InTheory);
	return true;
}

void UGSkillMasterySubsystem::HandleSkillProgressEvent(FGameplayTag ProgressEventTag, float Amount)
{
	if (!bSkillDataReady)
	{
		return;
	}

	if (!ProgressEventTag.IsValid())
	{
		return;
	}

	if (Amount <= 0.f)
	{
		return;
	}

	for (TPair<FGameplayTag, FSkillRuntimeState>& Pair : SkillStates)
	{
		const FGameplayTag SkillTag = Pair.Key;
		FSkillRuntimeState& State = Pair.Value;

		// Mastered/Locked Skill은 대상에서 제외 (InTheory만 누적)
		if (State.State != ESkillState::InTheory)
		{
			continue;
		}

		const UGSkillDefinition* Definition = SkillDefinitionCache.FindRef(SkillTag);
		if (!Definition)
		{
			continue;
		}

		const FSkillMasteryCondition* MatchedCondition = Definition->MasteryConditions.FindByPredicate(
			[ProgressEventTag](const FSkillMasteryCondition& Condition)
			{
				return Condition.ProgressEventTag == ProgressEventTag;
			});

		// 이 Skill이 요구하지 않는 이벤트면 건드리지 않는다.
		if (!MatchedCondition)
		{
			continue;
		}

		float& CurrentProgress = State.ConditionProgress.FindOrAdd(ProgressEventTag);
		const float NewProgress = FMath::Clamp(CurrentProgress + Amount, 0.f, MatchedCondition->RequiredAmount);

		if (FMath::IsNearlyEqual(NewProgress, CurrentProgress))
		{
			// 이미 RequiredAmount에 도달해 Clamp로 값이 변하지 않음 — 변화 없는 Broadcast 방지
			continue;
		}

		CurrentProgress = NewProgress;
		G_LOG(TEXT("Skill 시스템: [%s] Progress[%s] %.2f / %.2f"),
			*SkillTag.ToString(), *ProgressEventTag.ToString(), CurrentProgress, MatchedCondition->RequiredAmount);

		OnSkillProgressChanged.Broadcast(SkillTag, ProgressEventTag, CurrentProgress);

		TryMasterSkill(SkillTag);
	}
}

void UGSkillMasterySubsystem::TryMasterSkill(FGameplayTag SkillTag)
{
	FSkillRuntimeState* State = SkillStates.Find(SkillTag);
	if (!State || State->State != ESkillState::InTheory)
	{
		return;
	}

	const UGSkillDefinition* Definition = SkillDefinitionCache.FindRef(SkillTag);
	if (!Definition)
	{
		return;
	}

	// 정책: MasteryConditions가 비어 있는 Skill은 이 경로로 자동 Mastered 처리하지 않는다.
	if (Definition->MasteryConditions.IsEmpty())
	{
		return;
	}

	for (const FSkillMasteryCondition& Condition : Definition->MasteryConditions)
	{
		const float CurrentProgress = State->ConditionProgress.FindRef(Condition.ProgressEventTag);
		if (CurrentProgress < Condition.RequiredAmount)
		{
			return;
		}
	}

	// 선행 Skill 검사는 DiscoverSkill()의 진입 조건으로 이미 보장되어 여기서는 재검사하지 않는다.
	State->State = ESkillState::Mastered;
	G_LOG(TEXT("Skill 시스템: [%s] InTheory → Mastered"), *SkillTag.ToString());
	OnSkillStateChanged.Broadcast(SkillTag, ESkillState::Mastered);
}

bool UGSkillMasterySubsystem::ArePrerequisiteSkillsMastered(const UGSkillDefinition& Definition) const
{
	for (const FGameplayTag& RequiredSkillTag : Definition.RequiredSkills)
	{
		if (!RequiredSkillTag.IsValid())
		{
			G_WARN(TEXT("Skill 시스템: [%s] RequiredSkills에 Invalid 태그가 있어 Mastery 보류"),
				*Definition.SkillTag.ToString());
			return false;
		}

		if (!SkillDefinitionCache.Contains(RequiredSkillTag))
		{
			G_WARN(TEXT("Skill 시스템: [%s] 선행 Skill [%s]이 캐시에 존재하지 않아 Mastery 보류"),
				*Definition.SkillTag.ToString(), *RequiredSkillTag.ToString());
			return false;
		}

		if (!IsSkillMastered(RequiredSkillTag))
		{
			return false;
		}
	}

	return true;
}