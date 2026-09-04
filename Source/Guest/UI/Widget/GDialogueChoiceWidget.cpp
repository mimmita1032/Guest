// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GDialogueChoiceWidget.h"
#include "Guest/Utils/GuestBlueprintLibrary.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"

void UGDialogueChoiceWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Choice)
	{
		Btn_Choice->OnClicked().AddUObject(this, &UGDialogueChoiceWidget::OnClicked);
	}

	// 선택지 문장은 한 줄에 안 들어가는 경우가 흔하다 — 잘리는 대신 접히게 한다.
	UGuestBlueprintLibrary::ApplyAutoWrap(Text_ChoiceContent, ChoiceWrapTextAt);
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
