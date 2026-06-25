// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GBarDialogueWidget.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"

void UGBarDialogueWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (Btn_Next)
    {
        Btn_Next->OnClicked().AddUObject(this, &UGBarDialogueWidget::OnNextClicked);
    }
}

void UGBarDialogueWidget::NativeOnActivated()
{
    Super::NativeOnActivated();

    if (UGuestUISubsystem* UISys = GetUISubsystem())
    {
        CurrentBarData = UISys->GetPendingBarDialogueData();
    }

    CurrentLineIndex = 0;
    ShowCurrentLine();
}

void UGBarDialogueWidget::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();
    CurrentBarData = FBarDialogueData{};
    CurrentLineIndex = 0;
}

void UGBarDialogueWidget::ShowCurrentLine()
{
    if (!CurrentBarData.DialogueData.Lines.IsValidIndex(CurrentLineIndex)) return;

    const FNPCDialogueLine& Line = CurrentBarData.DialogueData.Lines[CurrentLineIndex];
    if (Text_SpeakerName) Text_SpeakerName->SetText(Line.SpeakerName);
    if (Text_DialogueLine) Text_DialogueLine->SetText(Line.DialogueText);
}

void UGBarDialogueWidget::OnNextClicked()
{
    ++CurrentLineIndex;

    if (CurrentBarData.DialogueData.Lines.IsValidIndex(CurrentLineIndex))
    {
        ShowCurrentLine();
    }
    else
    {
        DeactivateWidget();
    }
}
