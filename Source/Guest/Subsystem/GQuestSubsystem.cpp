// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GQuestSubsystem.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Data/DataAssets/GNarrationDataAsset.h"
#include "../Save/GuestSaveGame.h"

void UGQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	QuestDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/Quest/DT_QuestData")));
	if (QuestDataTable)
	{
		G_LOG(TEXT("퀘스트 데이터 테이블 로드 성공"));
		ValidateQuestDataTableIDs();
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

	// StepID가 비어 있으면 진행/저장 매칭이 모두 불가능하므로 수락 자체를 막고 데이터 입력을 유도
	if (Data->Steps[0].StepID.IsNone())
	{
		G_WARN(TEXT("퀘스트 시스템: [%s] 첫 단계의 StepID가 비어 있어 수락 불가 — DT_QuestData에 StepID 입력 필요"),
			*QuestID.ToString());
		return;
	}

	FQuestRuntimeData RuntimeData;
	RuntimeData.InitializeForStep(Data->Steps[0]);
	ActiveQuests.Add(QuestID, RuntimeData);

	G_LOG(TEXT("퀘스트 시스템: [%s] '%s' 수락 완료 (단계 %d개)"),
		*QuestID.ToString(), *Data->QuestName, Data->Steps.Num());

	OnQuestListChanged.Broadcast();
}

void UGQuestSubsystem::HandleObjectiveUpdated(FName TargetID, int32 Amount)
{
	TArray<FName> QuestsToComplete;
	for (auto& [QuestID, Runtime] : ActiveQuests)
	{
		const FQuestData* Data = FindQuestData(QuestID);
		if (!Data) continue;

		const int32 StepIndex = Data->FindStepIndexByID(Runtime.CurrentStepID);
		if (StepIndex == INDEX_NONE) continue;

		const FQuestStepData& CurrentStepData = Data->Steps[StepIndex];
		bool bAnyMatch = false;

		// 현재 단계의 매칭 목표 카운트 갱신
		for (const FQuestObjectiveData& Obj : CurrentStepData.Objectives)
		{
			if (Obj.TargetID != TargetID) continue;

			FQuestObjectiveProgress* Progress = Runtime.FindObjectiveProgress(Obj.ObjectiveID);
			if (!Progress) continue;

			Progress->Count = FMath::Min(Progress->Count + Amount, Obj.RequiredAmount);
			bAnyMatch = true;

			G_LOG(TEXT("퀘스트 시스템: [%s] 단계[%s] 목표[%s] %d / %d"),
				*QuestID.ToString(), *Runtime.CurrentStepID.ToString(),
				*TargetID.ToString(), Progress->Count, Obj.RequiredAmount);
		}

		if (!bAnyMatch) continue;

		// 현재 단계의 모든 목표 달성 여부 확인
		bool bStepComplete = true;
		for (const FQuestObjectiveData& Obj : CurrentStepData.Objectives)
		{
			if (Runtime.GetObjectiveCount(Obj.ObjectiveID) < Obj.RequiredAmount)
			{
				bStepComplete = false;
				break;
			}
		}

		if (!bStepComplete) continue;

		const int32 NextStepIndex = StepIndex + 1;
		if (!Data->Steps.IsValidIndex(NextStepIndex))
		{
			QuestsToComplete.Add(QuestID);
		}
		else
		{
			const FQuestStepData& NextStep = Data->Steps[NextStepIndex];
			if (NextStep.StepID.IsNone())
			{
				// CurrentStepID가 None이 되면 이후 목표 갱신이 전부 무시되어 퀘스트가 멈춤
				G_WARN(TEXT("퀘스트 시스템: [%s] 다음 단계(인덱스 %d)의 StepID가 비어 있음 — 진행이 멈추므로 DT_QuestData 입력 필요"),
					*QuestID.ToString(), NextStepIndex);
			}

			Runtime.InitializeForStep(NextStep);
			G_LOG(TEXT("퀘스트 시스템: [%s] 단계 완료 → 다음 단계 [%s]"),
				*QuestID.ToString(), *Runtime.CurrentStepID.ToString());

			OnQuestListChanged.Broadcast();
		}
	}
	// ActiveQuests 순회가 끝난 후 안전하게 제거
	for (const FName& QuestID : QuestsToComplete)
	{
		CompleteQuest(QuestID);
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

	if (!Data->NarrationOnComplete.IsNull())
	{
		if (UGNarrationDataAsset* NarrationAsset = Data->NarrationOnComplete.LoadSynchronous())
		{
			G_LOG(TEXT("퀘스트 시스템: [%s] 완료 나레이션 재생 요청"), *QuestID.ToString());
			OnNarrationRequested.Broadcast(NarrationAsset);
		}
	}

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

void UGQuestSubsystem::ValidateQuestDataTableIDs() const
{
	if (!QuestDataTable) return;

	for (const FName& RowName : QuestDataTable->GetRowNames())
	{
		const FQuestData* Data = QuestDataTable->FindRow<FQuestData>(RowName, TEXT("QuestIDValidation"), false);
		if (!Data) continue;

		TSet<FName> SeenStepIDs;
		for (int32 StepIndex = 0; StepIndex < Data->Steps.Num(); ++StepIndex)
		{
			const FQuestStepData& Step = Data->Steps[StepIndex];
			if (Step.StepID.IsNone())
			{
				G_WARN(TEXT("퀘스트 데이터 검증: [%s] 단계 %d의 StepID가 비어 있음 — 입력 전까지 진행/저장 불가"),
					*RowName.ToString(), StepIndex);
			}
			else if (SeenStepIDs.Contains(Step.StepID))
			{
				G_WARN(TEXT("퀘스트 데이터 검증: [%s] StepID [%s] 중복 — 복원 시 앞쪽 단계로만 매칭됨"),
					*RowName.ToString(), *Step.StepID.ToString());
			}
			SeenStepIDs.Add(Step.StepID);

			TSet<FName> SeenObjectiveIDs;
			for (int32 ObjIndex = 0; ObjIndex < Step.Objectives.Num(); ++ObjIndex)
			{
				const FName ObjectiveID = Step.Objectives[ObjIndex].ObjectiveID;
				if (ObjectiveID.IsNone())
				{
					G_WARN(TEXT("퀘스트 데이터 검증: [%s] 단계 %d 목표 %d의 ObjectiveID가 비어 있음 — 입력 전까지 진행/저장 불안정"),
						*RowName.ToString(), StepIndex, ObjIndex);
				}
				else if (SeenObjectiveIDs.Contains(ObjectiveID))
				{
					G_WARN(TEXT("퀘스트 데이터 검증: [%s] 단계 %d ObjectiveID [%s] 중복 — 진행도가 앞쪽 목표로만 매칭됨"),
						*RowName.ToString(), StepIndex, *ObjectiveID.ToString());
				}
				SeenObjectiveIDs.Add(ObjectiveID);
			}
		}
	}
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
		Entry.CurrentStepID = Pair.Value.CurrentStepID;
		Entry.ObjectiveProgress = Pair.Value.ObjectiveProgress;
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

		// 저장된 StepID가 현재 DataTable에 실제로 존재하는지 검증 (NAME_None 포함)
		const FQuestStepData* StepData = Data->FindStepByID(Entry.CurrentStepID);
		if (!StepData)
		{
			G_WARN(TEXT("퀘스트 복원 스킵(진행): [%s] StepID [%s]가 현재 DataTable에 없음"),
				*Entry.QuestID.ToString(),
				*Entry.CurrentStepID.ToString());

			continue;
		}

		// 현재 단계의 목표 기준으로 장부를 새로 만들고, 저장된 진행도를 ObjectiveID로 매칭해 복원
		FQuestRuntimeData Runtime;
		Runtime.InitializeForStep(*StepData);

		for (const FQuestObjectiveProgress& Saved : Entry.ObjectiveProgress)
		{
			const FQuestObjectiveData* Obj = StepData->FindObjectiveByID(Saved.ObjectiveID);
			FQuestObjectiveProgress* Target = Runtime.FindObjectiveProgress(Saved.ObjectiveID);
			if (!Obj || !Target)
			{
				G_WARN(TEXT("퀘스트 복원: [%s] 목표 진행도 스킵 — ObjectiveID [%s]가 단계 [%s]에 없음"),
					*Entry.QuestID.ToString(),
					*Saved.ObjectiveID.ToString(),
					*Entry.CurrentStepID.ToString());
				continue;
			}

			Target->Count = FMath::Clamp(Saved.Count, 0, Obj->RequiredAmount);
		}

		ActiveQuests.Add(Entry.QuestID, MoveTemp(Runtime));
	}
	OnQuestListChanged.Broadcast();
}
#pragma endregion
