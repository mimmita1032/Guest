// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GuestNPCDialogueWidget.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"

void UGuestNPCDialogueWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Next)
	{
		Btn_Next->OnClicked().AddUObject(this, &UGuestNPCDialogueWidget::OnNextClicked);
	}
}

void UGuestNPCDialogueWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (UGuestUISubsystem* UISys = GetUISubsystem())
	{
		CurrentDialogueData = UISys->GetPendingDialogueData();
	}

	CurrentLineIndex = 0;
	ShowCurrentLine();
}

void UGuestNPCDialogueWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	CurrentDialogueData = FNPCDialogueData{};
	CurrentLineIndex = 0;
}

void UGuestNPCDialogueWidget::ShowCurrentLine()
{
	if (!CurrentDialogueData.Lines.IsValidIndex(CurrentLineIndex)) return;

	const FNPCDialogueLine& Line = CurrentDialogueData.Lines[CurrentLineIndex];
	if (Text_SpeakerName) Text_SpeakerName->SetText(Line.SpeakerName);
	if (Text_DialogueLine) Text_DialogueLine->SetText(Line.DialogueText);
}

void UGuestNPCDialogueWidget::OnNextClicked()
{
	++CurrentLineIndex;

	if (CurrentDialogueData.Lines.IsValidIndex(CurrentLineIndex))
	{
		ShowCurrentLine();
	}
	else
	{
		DeactivateWidget();
	}
}
