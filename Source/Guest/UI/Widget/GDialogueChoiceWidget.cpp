// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GDialogueChoiceWidget.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"

void UGDialogueChoiceWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Choice)
	{
		Btn_Choice->OnClicked().AddUObject(this, &UGDialogueChoiceWidget::OnClicked);
	}
}

void UGDialogueChoiceWidget::SetupChoice(const FDialogueChoice& Choice)
{
	CachedNextNodeID = Choice.NextNodeID;
	if (Text_ChoiceContent)
	{
		Text_ChoiceContent->SetText(FText::FromString(Choice.ChoiceText));
	}
}

void UGDialogueChoiceWidget::OnClicked()
{
	OnChoiceSelected.Broadcast(CachedNextNodeID);
}
