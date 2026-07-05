// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GQuestSubsystem.h"
#include "Guest/Utils/GLog.h"
#include "../Save/GuestSaveGame.h"

void UGQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	QuestDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/Quest/DT_QuestData")));
	if (QuestDataTable)
	{
		G_LOG(TEXT("퀘스트 데이터 테이블 로드 성공"));
	}
	else
	{
		G_ERR(TEXT("퀘스트 데이터 테이블 로드 실패! 경로를 다시 체크: /Game/Data/Quest/DT_QuestData"));
	}

	OnObjectiveUpdated.AddDynamic(this, &UGQuestSubsystem::HandleObjectiveUpdated);
}

#pragma region Quest Flow
void UGQuestSubsystem::AcceptQuest(FName QuestID)
{
	const FQuestData* Data = FindQuestData(QuestID);
	if (!Data)
	{
		G_WARN(TEXT("퀘스트 시스템: [%s] 데이터를 찾을 수 없어 수락 불가"), *QuestID.ToString());
		return;
	}

	if (IsQuestActive(QuestID))
	{
		G_WARN(TEXT("퀘스트 시스템: [%s] 이미 진행 중인 퀘스트"), *QuestID.ToString());
		return;
	}

	if (IsQuestCompleted(QuestID))
	{
		G_WARN(TEXT("퀘스트 시스템: [%s] 이미 완료한 퀘스트"), *QuestID.ToString());
		return;
	}

	if (!Data->RequiredQuestID.IsNone() && !IsQuestCompleted(Data->RequiredQuestID))
	{
		G_WARN(TEXT("퀘스트 시스템: [%s] 선행 퀘스트 [%s] 미완료로 수락 불가"),
			*QuestID.ToString(), *Data->RequiredQuestID.ToString());
		return;
	}

	if (StoryProgress < Data->RequiredStoryProgress)
	{
		G_WARN(TEXT("퀘스트 시스템: [%s] 스토리 진행도 부족 (현재 %d / 필요 %d)으로 수락 불가"),
			*QuestID.ToString(), StoryProgress, Data->RequiredStoryProgress);
		return;
	}

	// TODO: RequiredTimeState 검증 (GSpacetimeSubsystem 연동 필요)

	if (Data->Steps.Num() == 0)
	{
		G_WARN(TEXT("퀘스트 시스템: [%s] Steps가 비어있어 수락 불가"), *QuestID.ToString());
		return;
	}

	FQuestRuntimeData RuntimeData;
	RuntimeData.CurrentStep = 0;
	RuntimeData.ObjectiveCounts.SetNumZeroed(Data->Steps[0].Objectives.Num());
	ActiveQuests.Add(QuestID, RuntimeData);

	G_LOG(TEXT("퀘스트 시스템: [%s] '%s' 수락 완료 (단계 %d개)"),
		*QuestID.ToString(), *Data->QuestName, Data->Steps.Num());

	OnQuestListChanged.Broadcast();
}

void UGQuestSubsystem::HandleObjectiveUpdated(FName TargetID, int32 Amount)
{
	for (auto& [QuestID, Runtime] : ActiveQuests)
	{
		const FQuestData* Data = FindQuestData(QuestID);
		if (!Data || !Data->Steps.IsValidIndex(Runtime.CurrentStep)) continue;

		const FQuestStepData& CurrentStepData = Data->Steps[Runtime.CurrentStep];
		bool bAnyMatch = false;

		// 현재 단계의 매칭 목표 카운트 갱신
		for (int32 i = 0; i < CurrentStepData.Objectives.Num(); ++i)
		{
			const FQuestObjectiveData& Obj = CurrentStepData.Objectives[i];
			if (Obj.TargetID != TargetID) continue;

			Runtime.ObjectiveCounts[i] = FMath::Min(
				Runtime.ObjectiveCounts[i] + Amount, Obj.RequiredAmount);
			bAnyMatch = true;

			G_LOG(TEXT("퀘스트 시스템: [%s] 단계%d 목표[%s] %d / %d"),
				*QuestID.ToString(), Runtime.CurrentStep,
				*TargetID.ToString(), Runtime.ObjectiveCounts[i], Obj.RequiredAmount);
		}

		if (!bAnyMatch) continue;

		// 현재 단계의 모든 목표 달성 여부 확인
		bool bStepComplete = true;
		for (int32 i = 0; i < CurrentStepData.Objectives.Num(); ++i)
		{
			if (Runtime.ObjectiveCounts[i] < CurrentStepData.Objectives[i].RequiredAmount)
			{
				bStepComplete = false;
				break;
			}
		}

		if (!bStepComplete) continue;

		Runtime.CurrentStep++;
		G_LOG(TEXT("퀘스트 시스템: [%s] 단계 완료 → 다음 단계 %d"),
			*QuestID.ToString(), Runtime.CurrentStep);

		if (Runtime.CurrentStep >= Data->Steps.Num())
		{
			CompleteQuest(QuestID);
		}
		else
		{
			// 다음 단계 목표 수량에 맞게 카운트 초기화
			Runtime.ObjectiveCounts.SetNumZeroed(Data->Steps[Runtime.CurrentStep].Objectives.Num());
			OnQuestListChanged.Broadcast();
		}
	}
}

void UGQuestSubsystem::CompleteQuest(FName QuestID)
{
	const FQuestData* Data = FindQuestData(QuestID);
	if (!Data) return;

	ActiveQuests.Remove(QuestID);
	CompletedQuests.Add(QuestID);

	G_LOG(TEXT("퀘스트 시스템: [%s] '%s' 완료!"), *QuestID.ToString(), *Data->QuestName);

	// TODO: RewardItemID 기반 보상 아이템 인벤토리에 지급
	if (!Data->RewardItemID.IsNone())
	{
		G_LOG(TEXT("퀘스트 시스템: 보상 아이템 [%s] 지급 예정"), *Data->RewardItemID.ToString());
	}

	if (Data->StoryProgressOnComplete > StoryProgress)
	{
		SetStoryProgress(Data->StoryProgressOnComplete);
	}

	OnQuestListChanged.Broadcast();

	if (!Data->NextQuestID.IsNone())
	{
		G_LOG(TEXT("퀘스트 시스템: 연결 퀘스트 [%s] 자동 수락"), *Data->NextQuestID.ToString());
		AcceptQuest(Data->NextQuestID);
	}
}
#pragma endregion

#pragma region Story Progress
void UGQuestSubsystem::SetStoryProgress(int32 NewProgress)
{
	if (NewProgress == StoryProgress) return;

	StoryProgress = NewProgress;
	G_LOG(TEXT("퀘스트 시스템: 스토리 진행도 → %d"), StoryProgress);

	OnQuestListChanged.Broadcast();
}
#pragma endregion

#pragma region Helpers
bool UGQuestSubsystem::IsQuestActive(FName QuestID) const
{
	return ActiveQuests.Contains(QuestID);
}

bool UGQuestSubsystem::IsQuestCompleted(FName QuestID) const
{
	return CompletedQuests.Contains(QuestID);
}

TArray<FName> UGQuestSubsystem::GetActiveQuestIDs() const
{
	TArray<FName> Result;
	ActiveQuests.GetKeys(Result);
	return Result;
}

TArray<FName> UGQuestSubsystem::GetCompletedQuestIDs() const
{
	return CompletedQuests.Array();
}

bool UGQuestSubsystem::GetQuestRuntimeData(FName QuestID, FQuestRuntimeData& OutData) const
{
	if (const FQuestRuntimeData* Found = ActiveQuests.Find(QuestID))
	{
		OutData = *Found;
		return true;
	}
	return false;
}

const FQuestData* UGQuestSubsystem::FindQuestData(FName QuestID) const
{
	if (!QuestDataTable) return nullptr;
	return QuestDataTable->FindRow<FQuestData>(QuestID, TEXT("QuestLookup"));
}
#pragma endregion

#pragma region Save

void UGQuestSubsystem::ExportQuestSaveData(TArray<FGuestSavedActiveQuestEntry>& OutActiveQuests, TArray<FName>& OutCompletedQuestIDs) const
{
	OutActiveQuests.Reset();
	OutCompletedQuestIDs.Reset();

	OutActiveQuests.Reserve(ActiveQuests.Num());
	for (const TPair<FName, FQuestRuntimeData>& Pair : ActiveQuests)
	{
		FGuestSavedActiveQuestEntry Entry;
		Entry.QuestID = Pair.Key;
		Entry.CurrentStep = Pair.Value.CurrentStep;
		Entry.ObjectiveCounts = Pair.Value.ObjectiveCounts;
		OutActiveQuests.Add(MoveTemp(Entry));
	}
	OutCompletedQuestIDs = CompletedQuests.Array();
}

void UGQuestSubsystem::ImportQuestSaveData(const TArray<FGuestSavedActiveQuestEntry>& InActiveQuests, const TArray<FName>& InCompletedQuestIDs)
{
	ActiveQuests.Reset();
	CompletedQuests.Reset();

	// DataTable에 존재하는 완료 퀘스트만 복원
	for (const FName& QuestID : InCompletedQuestIDs)
	{
		if (QuestID.IsNone()) continue;
		if (!FindQuestData(QuestID))
		{
			G_WARN(TEXT("퀘스트 복원 스킵(완료): [%s] DataTable에 없음"), *QuestID.ToString());
			continue;
		}
		CompletedQuests.Add(QuestID);
	}

	// 활성 퀘스트는 현재 데이터 구조에 맞는 항목만 복원
	for (const FGuestSavedActiveQuestEntry& Entry : InActiveQuests)
	{
		if (Entry.QuestID.IsNone()) continue;

		const FQuestData* Data = FindQuestData(Entry.QuestID);
		if (!Data)
		{
			G_WARN(TEXT("퀘스트 복원 스킵(진행): [%s] DataTable에 없음"), *Entry.QuestID.ToString());
			continue;
		}

		if (Data->Steps.IsEmpty())
		{
			G_WARN(TEXT("퀘스트 복원 스킵(진행): [%s] Steps가 비어 있음"),
				*Entry.QuestID.ToString());
			continue;
		}

		if (CompletedQuests.Contains(Entry.QuestID))
		{
			G_WARN(TEXT("퀘스트 복원 스킵(진행): [%s] 완료 목록과 중복"),
				*Entry.QuestID.ToString());
			continue;
		}

		// 중복 저장 데이터는 먼저 정상 복원된 항목을 유지
		if (ActiveQuests.Contains(Entry.QuestID))
		{
			G_WARN(TEXT("퀘스트 복원 스킵(진행): [%s] 활성 목록에서 중복"),
				*Entry.QuestID.ToString());
			continue;
		}

		if (!Data->Steps.IsValidIndex(Entry.CurrentStep))
		{
			G_WARN(
				TEXT("퀘스트 복원 스킵(진행): [%s] CurrentStep=%d, 유효 범위=0~%d"),
				*Entry.QuestID.ToString(),
				Entry.CurrentStep,
				Data->Steps.Num() - 1);

			continue;
		}

		// 검증된 단계와 목표 진행도를 런타임 데이터로 복원
		FQuestRuntimeData Runtime;
		Runtime.CurrentStep = Entry.CurrentStep;

		const int32 ObjectiveNum = Data->Steps.IsValidIndex(Runtime.CurrentStep)
			? Data->Steps[Runtime.CurrentStep].Objectives.Num() : 0;
		Runtime.ObjectiveCounts = Entry.ObjectiveCounts;
		Runtime.ObjectiveCounts.SetNumZeroed(ObjectiveNum);

		const FQuestStepData& CurrentStep = Data->Steps[Runtime.CurrentStep];

		for (int32 Index = 0; Index < ObjectiveNum; ++Index)
		{
			Runtime.ObjectiveCounts[Index] = FMath::Clamp(
				Runtime.ObjectiveCounts[Index],
				0,
				CurrentStep.Objectives[Index].RequiredAmount);
		}

		ActiveQuests.Add(Entry.QuestID, MoveTemp(Runtime));
	}
}
#pragma endregion
