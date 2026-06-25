// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GBarDialogueWidget.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/Characters/NPC/BarCustomerNPC.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"
#include "GameFramework/PlayerController.h"

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

    UGuestUISubsystem* UISys = GetUISubsystem();
    if (!UISys) return;

    CurrentBarData = UISys->GetPendingBarDialogueData();
    CurrentLineIndex = 0;
    ShowCurrentLine();

    if (ABarCustomerNPC* NPC = Cast<ABarCustomerNPC>(UISys->GetPendingBarDialogueActor()))
    {
        BlendToNPCCamera(NPC);
    }
}

void UGBarDialogueWidget::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();

    UGuestUISubsystem* UISys = GetUISubsystem();
    if (UISys)
    {
        if (ABarCustomerNPC* NPC = Cast<ABarCustomerNPC>(UISys->GetPendingBarDialogueActor()))
        {
            NPC->EndBarDialogue();
        }
    }

    CurrentBarData = FBarDialogueData{};
    CurrentLineIndex = 0;
}

void UGBarDialogueWidget::BlendToNPCCamera(ABarCustomerNPC* NPC)
{
    if (!NPC) return;

    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    PC->SetViewTargetWithBlend(NPC, 0.5f);
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
