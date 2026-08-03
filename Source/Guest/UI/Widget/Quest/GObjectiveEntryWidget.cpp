// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/Quest/GObjectiveEntryWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

void UGObjectiveEntryWidget::Setup(const FString& InObjectiveText, bool bCompleted)
{
	if (Text_Objective) Text_Objective->SetText(FText::FromString(InObjectiveText));
	if (Check_Done)
	{
		Check_Done->SetIsChecked(bCompleted);
		Check_Done->SetIsEnabled(false);
	}
}
