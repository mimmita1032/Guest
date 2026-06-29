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
		if (!Data->Steps.IsValidIndex(Runtime.CurrentStep)) continue;

		const FQuestStepData& CurrentStep = Data->Steps[Runtime.CurrentStep];

		for (int32 i = 0; i < CurrentStep.Objectives.Num(); ++i)
		{
			UGObjectiveEntryWidget* Entry = CreateWidget<UGObjectiveEntryWidget>(this, ObjectiveEntryClass);
			if (!Entry) continue;

			const bool bDone = Runtime.ObjectiveCounts.IsValidIndex(i)
				&& Runtime.ObjectiveCounts[i] >= CurrentStep.Objectives[i].RequiredAmount;

			Entry->Setup(CurrentStep.Objectives[i].ObjectiveText, bDone);
			Box_QuestList->AddChild(Entry);
		}
	}
}

UGQuestSubsystem* UGQuestTrackerWidget::GetQuestSubsystem() const
{
	UGameInstance* GI = GetGameInstance();
	return GI ? GI->GetSubsystem<UGQuestSubsystem>() : nullptr;
}
