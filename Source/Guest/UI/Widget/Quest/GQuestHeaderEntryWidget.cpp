// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GQuestHeaderEntryWidget.h"
#include "Components/TextBlock.h"

void UGQuestHeaderEntryWidget::Setup(const FString& InQuestName, const FString& InDescription)
{
	if (Text_QuestName)
	{
		Text_QuestName->SetText(FText::FromString(InQuestName));
	}

	if (Text_Description)
	{
		Text_Description->SetText(FText::FromString(InDescription));
	}
}
