// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/Quest/GQuestTrackerWidget.h"
#include "Guest/UI/Widget/Quest/GObjectiveEntryWidget.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/Data/DataTable/GQuestTypes.h"
#include "Components/VerticalBox.h"

void UGQuestTrackerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UGQuestSubsystem* QuestSys = GetQuestSubsystem())
	{
		QuestSys->OnQuestListChanged.AddDynamic(this, &UGQuestTrackerWidget::RefreshQuestList);
	}

	// 구독 시점에 이미 진행 중인 퀘스트가 있으면 즉시 반영 (BeginPlay 순서상 브로드캐스트를 놓칠 수 있음)
	RefreshQuestList();
}

void UGQuestTrackerWidget::RefreshQuestList()
{
	if (!Box_QuestList || !ObjectiveEntryClass) return;
	Box_QuestList->ClearChildren();

	UGQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys) return;

	for (const FName& QuestID : QuestSys->GetActiveQuestIDs())
	{
		const FQuestData* Data = QuestSys->FindQuestDataPublic(QuestID);
		FQuestRuntimeData Runtime;
		if (!Data || !QuestSys->GetQuestRuntimeData(QuestID, Runtime)) continue;
		const FQuestStepData* CurrentStep = Data->FindStepByID(Runtime.CurrentStepID);
		if (!CurrentStep) continue;

		for (const FQuestObjectiveData& Objective : CurrentStep->Objectives)
		{
			UGObjectiveEntryWidget* Entry = CreateWidget<UGObjectiveEntryWidget>(this, ObjectiveEntryClass);
			if (!Entry) continue;

			const bool bDone = Runtime.GetObjectiveCount(Objective.ObjectiveID) >= Objective.RequiredAmount;

			Entry->Setup(Objective.ObjectiveText, bDone);
			Box_QuestList->AddChild(Entry);
		}
	}
}

UGQuestSubsystem* UGQuestTrackerWidget::GetQuestSubsystem() const
{
	UGameInstance* GI = GetGameInstance();
	return GI ? GI->GetSubsystem<UGQuestSubsystem>() : nullptr;
}
