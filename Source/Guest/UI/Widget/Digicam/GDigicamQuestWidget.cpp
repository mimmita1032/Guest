// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GDigicamQuestWidget.h"
#include "Guest/UI/Widget/Quest/GObjectiveEntryWidget.h"
#include "Guest/UI/Widget/Quest/GQuestHeaderEntryWidget.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/Data/DataTable/GQuestTypes.h"
#include "Components/VerticalBox.h"
#include "Components/Widget.h"

void UGDigicamQuestWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UGQuestSubsystem* QuestSys = GetQuestSubsystem())
	{
		QuestSys->OnQuestListChanged.AddDynamic(this, &UGDigicamQuestWidget::RefreshQuestList);
	}
}

void UGDigicamQuestWidget::OnTabActivated_Implementation()
{
	RefreshQuestList();
}

void UGDigicamQuestWidget::RefreshQuestList()
{
	if (!Box_QuestList || !ObjectiveEntryClass || !QuestHeaderClass) return;
	Box_QuestList->ClearChildren();

	UGQuestSubsystem* QuestSys = GetQuestSubsystem();
	const TArray<FName> ActiveIDs = QuestSys ? QuestSys->GetActiveQuestIDs() : TArray<FName>();

	if (WGT_EmptyHint)
	{
		WGT_EmptyHint->SetVisibility(ActiveIDs.IsEmpty() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	for (const FName& QuestID : ActiveIDs)
	{
		const FQuestData* Data = QuestSys->FindQuestDataPublic(QuestID);
		FQuestRuntimeData Runtime;
		if (!Data || !QuestSys->GetQuestRuntimeData(QuestID, Runtime)) continue;
		if (!Data->Steps.IsValidIndex(Runtime.CurrentStep)) continue;

		// 퀘스트 제목 + 설명 헤더
		if (UGQuestHeaderEntryWidget* Header = CreateWidget<UGQuestHeaderEntryWidget>(this, QuestHeaderClass))
		{
			Header->Setup(Data->QuestName, Data->Description);
			Box_QuestList->AddChild(Header);
		}

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

UGQuestSubsystem* UGDigicamQuestWidget::GetQuestSubsystem() const
{
	const UGameInstance* GI = GetGameInstance();
	return GI ? GI->GetSubsystem<UGQuestSubsystem>() : nullptr;
}
