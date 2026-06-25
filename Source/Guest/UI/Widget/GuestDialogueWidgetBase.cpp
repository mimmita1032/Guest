// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GuestDialogueWidgetBase.h"
#include "Guest/Data/DataAssets/GDialogueDataAsset.h"
#include "Guest/Data/DataTable/GDialogueTypes.h"
#include "Guest/UI/Widget/GDialogueChoiceWidget.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"
#include "Components/VerticalBox.h"

void UGuestDialogueWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Next)
	{
		Btn_Next->OnClicked().AddUObject(this, &UGuestDialogueWidgetBase::OnNextClicked);
	}
}

void UGuestDialogueWidgetBase::StartDialogueSession(UGDialogueDataAsset* InAsset)
{
	if (!InAsset) return;
	CurrentDialogueAsset = InAsset;
	CurrentNodeID = InAsset->StartNodeID;
	ShowCurrentNode();
}

void UGuestDialogueWidgetBase::ResetDialogueSession()
{
	CurrentDialogueAsset = nullptr;
	CurrentNodeID = NAME_None;
	if (Box_Choices) Box_Choices->ClearChildren();
}

void UGuestDialogueWidgetBase::ShowCurrentNode()
{
	if (!CurrentDialogueAsset) return;

	const FDialogueNode* Node = CurrentDialogueAsset->FindNode(CurrentNodeID);
	if (!Node)
	{
		DeactivateWidget();
		return;
	}

	if (Text_SpeakerName)  Text_SpeakerName->SetText(Node->SpeakerName);
	if (Text_DialogueLine) Text_DialogueLine->SetText(Node->DialogueText);

	if (Node->Choices.Num() > 0)
	{
		if (Btn_Next) Btn_Next->SetVisibility(ESlateVisibility::Collapsed);
		PopulateChoices(Node->Choices);
	}
	else
	{
		if (Box_Choices)
		{
			Box_Choices->ClearChildren();
			Box_Choices->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (Btn_Next) Btn_Next->SetVisibility(ESlateVisibility::Visible);
		if (Text_PlayerResponse)
		{
			if (!Node->PlayerResponseText.IsEmpty())
			{
				Text_PlayerResponse->SetText(Node->PlayerResponseText);
			}
			else
			{
				const FText DefaultText = Node->NextNodeID.IsNone()
					? FText::FromString(TEXT("대화 종료"))
					: FText::FromString(TEXT("계속"));
				Text_PlayerResponse->SetText(DefaultText);
			}
		}
	}
}

void UGuestDialogueWidgetBase::PopulateChoices(const TArray<FDialogueChoice>& Choices)
{
	if (!Box_Choices || !ChoiceWidgetClass) return;

	Box_Choices->ClearChildren();
	Box_Choices->SetVisibility(ESlateVisibility::Visible);

	for (const FDialogueChoice& Choice : Choices)
	{
		UGDialogueChoiceWidget* ChoiceWidget =
			CreateWidget<UGDialogueChoiceWidget>(this, ChoiceWidgetClass);
		if (!ChoiceWidget) continue;

		ChoiceWidget->SetupChoice(Choice);
		ChoiceWidget->OnChoiceSelected.AddUObject(this, &UGuestDialogueWidgetBase::OnChoiceSelected);
		Box_Choices->AddChild(ChoiceWidget);
	}
}

void UGuestDialogueWidgetBase::AdvanceTo(FName NextNodeID)
{
	if (NextNodeID.IsNone())
	{
		DeactivateWidget();
		return;
	}
	CurrentNodeID = NextNodeID;
	ShowCurrentNode();
}

void UGuestDialogueWidgetBase::OnNextClicked()
{
	if (!CurrentDialogueAsset) return;

	const FDialogueNode* Node = CurrentDialogueAsset->FindNode(CurrentNodeID);
	if (!Node)
	{
		DeactivateWidget();
		return;
	}

	AdvanceTo(Node->NextNodeID);
}

void UGuestDialogueWidgetBase::OnChoiceSelected(FName NextNodeID)
{
	AdvanceTo(NextNodeID);
}
