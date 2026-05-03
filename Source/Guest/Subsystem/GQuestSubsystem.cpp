// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GQuestSubsystem.h"
#include "Guest/Utils/GLog.h"
#include "../Save/GuestSaveGame.h"

void UGQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	QuestDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Core/Data/DT_QuestData")));
	if (QuestDataTable)
	{
		G_LOG(TEXT("퀘스트 데이터 테이블 로드 성공"));
	}
	else
	{
		G_ERR(TEXT("퀘스트 데이터 테이블 로드 실패! 경로를 다시 체크: /Game/Core/Data/DT_QuestData"));
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

	// 선행 퀘스트 검증
	if (!Data->RequiredQuestID.IsNone() && !IsQuestCompleted(Data->RequiredQuestID))
	{
		G_WARN(TEXT("퀘스트 시스템: [%s] 선행 퀘스트 [%s] 미완료로 수락 불가"),
			*QuestID.ToString(), *Data->RequiredQuestID.ToString());
		return;
	}

	// TODO: RequiredTimeState 검증 (GSpacetimeSubsystem과 연동 필요)
	// EQuestTimeState::Past / Present 조건을 현재 레벨 시간대와 비교

	// 진행 장부에 등록 - 목표 수량 배열을 Objectives 크기에 맞게 초기화
	FQuestRuntimeData RuntimeData;
	RuntimeData.CurrentStep = 0;
	RuntimeData.ObjectiveCounts.SetNum(Data->Objectives.Num());
	ActiveQuests.Add(QuestID, RuntimeData);

	G_LOG(TEXT("퀘스트 시스템: [%s] '%s' 수락 완료 (목표 %d개)"),
		*QuestID.ToString(), *Data->QuestName.ToString(), Data->Objectives.Num());

	// TODO: UI 서브시스템에 퀘스트 수락 알림 전달
}

void UGQuestSubsystem::HandleObjectiveUpdated(FName TargetID, int32 Amount)
{
	for (auto& [QuestID, Runtime] : ActiveQuests)
	{
		const FQuestData* Data = FindQuestData(QuestID);
		if (!Data || !Data->Objectives.IsValidIndex(Runtime.CurrentStep)) continue;

		const FQuestObjectiveData& CurrentObjective = Data->Objectives[Runtime.CurrentStep];
		if (CurrentObjective.TargetID != TargetID) continue;

		Runtime.ObjectiveCounts[Runtime.CurrentStep] += Amount;

		G_LOG(TEXT("퀘스트 시스템: 대상 [%s] %d개 갱신 완료 (%d / %d)"),
			*TargetID.ToString(), Amount,
			Runtime.ObjectiveCounts[Runtime.CurrentStep],
			CurrentObjective.RequiredAmount);

		// 현재 목표 달성 여부 확인
		if (Runtime.ObjectiveCounts[Runtime.CurrentStep] >= CurrentObjective.RequiredAmount)
		{
			Runtime.CurrentStep++;
			G_LOG(TEXT("퀘스트 시스템: [%s] 목표 완료 → 다음 단계 %d"),
				*QuestID.ToString(), Runtime.CurrentStep);

			// 모든 목표 완료 시 퀘스트 완료 처리
			if (Runtime.CurrentStep >= Data->Objectives.Num())
			{
				CompleteQuest(QuestID);
			}
		}

		// 퀘스트 하나에만 갱신되도록 동일 TargetID라도 첫 번째 매칭만 처리
		break;
	}
}

void UGQuestSubsystem::CompleteQuest(FName QuestID)
{
	const FQuestData* Data = FindQuestData(QuestID);
	if (!Data) return;

	ActiveQuests.Remove(QuestID);
	CompletedQuests.Add(QuestID);

	G_LOG(TEXT("퀘스트 시스템: [%s] '%s' 완료!"), *QuestID.ToString(), *Data->QuestName.ToString());

	// TODO: RewardItemID 기반 보상 아이템 인벤토리에 지급
	if (!Data->RewardItemID.IsNone())
	{
		G_LOG(TEXT("퀘스트 시스템: 보상 아이템 [%s] 지급 예정"), *Data->RewardItemID.ToString());
	}

	// TODO: UI 서브시스템에 퀘스트 완료 팝업 알림 전달

	// 다음 퀘스트 자동 연결
	if (!Data->NextQuestID.IsNone())
	{
		G_LOG(TEXT("퀘스트 시스템: 연결 퀘스트 [%s] 자동 수락"), *Data->NextQuestID.ToString());
		AcceptQuest(Data->NextQuestID);
	}
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
	for (const TPair<FName, FQuestRuntimeData>& pair : ActiveQuests)
	{
		FGuestSavedActiveQuestEntry Entry;
		Entry.QuestID = pair.Key;
		Entry.CurrentStep = pair.Value.CurrentStep;
		Entry.ObjectiveCounts = pair.Value.ObjectiveCounts;
		OutActiveQuests.Add(MoveTemp(Entry));
	}
	OutCompletedQuestIDs = CompletedQuests.Array();
}

void UGQuestSubsystem::ImportQuestSaveData(const TArray<FGuestSavedActiveQuestEntry>& InActiveQuests, const TArray<FName>& InCompletedQuestIDs)
{
	ActiveQuests.Reset();
	CompletedQuests.Reset();
	
	//완료 목록 복원
	for (const FName& QuestID : InCompletedQuestIDs)
	{
		if (QuestID.IsNone())
		{
			continue;
		}
		
		if (!FindQuestData(QuestID))
		{
			G_WARN(TEXT("퀘스트 복원 스킵(완료): [%s] DataTable에 없음"), *QuestID.ToString());
			continue;
		}
		
		CompletedQuests.Add(QuestID);
	}
	
	for (const FGuestSavedActiveQuestEntry& Entry : InActiveQuests)
	{
		if (Entry.QuestID.IsNone())
		{
			continue;
		}
		
		const FQuestData* Data = FindQuestData(Entry.QuestID);
		if (!Data)
		{
			G_WARN(TEXT("퀘스트 복원 스킵(진행): [%s] DataTable에 없음"), *Entry.QuestID.ToString());
			continue;
		}
		
		if (CompletedQuests.Contains(Entry.QuestID))
		{
			continue;
		}
		
		FQuestRuntimeData Runtime;
		const int32 ObjectiveNum = Data -> Objectives.Num();
		
		Runtime.CurrentStep = FMath::Clamp(Entry.CurrentStep, 0, FMath::Max(0, ObjectiveNum - 1));
		
		Runtime.ObjectiveCounts = Entry.ObjectiveCounts;
		Runtime.ObjectiveCounts.SetNumZeroed(ObjectiveNum);
		
		ActiveQuests.Add(Entry.QuestID, MoveTemp(Runtime));
	}
}
#pragma endregion 