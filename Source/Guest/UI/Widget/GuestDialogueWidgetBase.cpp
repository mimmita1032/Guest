// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GuestDialogueWidgetBase.h"
#include "Guest/Data/DataAssets/GDialogueDataAsset.h"
#include "Guest/Data/DataTable/GDialogueTypes.h"
#include "Guest/UI/Widget/GDialogueChoiceWidget.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

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

	if (Text_SpeakerName)  Text_SpeakerName->SetText(FText::FromString(Node->SpeakerName));
	if (Text_DialogueLine) Text_DialogueLine->SetText(FText::FromString(Node->DialogueText));

	if (!Node->QuestEventID.IsNone())
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UGQuestSubsystem* QuestSys = GI->GetSubsystem<UGQuestSubsystem>())
			{
				QuestSys->AcceptQuest(Node->QuestEventID);
			}
		}
	}

	if (Node->Choices.Num() > 0)
	{
		if (Btn_Next) Btn_Next->SetVisibility(ESlateVisibility::Collapsed);
		if (Text_PlayerResponse) Text_PlayerResponse->SetVisibility(ESlateVisibility::Collapsed);
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
			// 선택지 노드를 지나오면 Collapsed로 남아 있다 — 텍스트만 채우면 화면에 나오지 않는다.
			// Visible이 아니라 SelfHitTestInvisible이어야 한다. 이 텍스트는 Btn_Next 위에 얹히므로
			// 히트 테스트 대상이 되면 클릭을 자기가 먹고 버튼이 눌리지 않는다.
			Text_PlayerResponse->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (!Node->PlayerResponseText.IsEmpty())
			{
				Text_PlayerResponse->SetText(FText::FromString(Node->PlayerResponseText));
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

bool UGuestDialogueWidgetBase::IsChoiceConditionMet(FName ConditionID) const
{
	// 조건이 없으면 항상 표시
	if (ConditionID.IsNone()) return true;

	const UWorld* World = GetWorld();
	const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	const UGQuestSubsystem* QuestSys = GI ? GI->GetSubsystem<UGQuestSubsystem>() : nullptr;
	if (!QuestSys) return false;

	// "QuestID.StepID" — 단계까지 일치해야 한다. 점이 없으면 단계 무관.
	FString QuestPart, StepPart;
	if (ConditionID.ToString().Split(TEXT("."), &QuestPart, &StepPart))
	{
		return QuestSys->GetCurrentStepID(FName(*QuestPart)) == FName(*StepPart);
	}

	return QuestSys->IsQuestActive(ConditionID);
}

void UGuestDialogueWidgetBase::PopulateChoices(const TArray<FDialogueChoice>& Choices)
{
	if (!Box_Choices || !ChoiceWidgetClass) return;

	Box_Choices->ClearChildren();
	Box_Choices->SetVisibility(ESlateVisibility::Visible);

	for (const FDialogueChoice& Choice : Choices)
	{
		if (!IsChoiceConditionMet(Choice.ConditionID)) continue;

		UGDialogueChoiceWidget* ChoiceWidget =
			CreateWidget<UGDialogueChoiceWidget>(this, ChoiceWidgetClass);
		if (!ChoiceWidget) continue;

		ChoiceWidget->SetupChoice(Choice);
		ChoiceWidget->OnChoiceSelected.AddUObject(this, &UGuestDialogueWidgetBase::OnChoiceSelected);

		if (UVerticalBoxSlot* VSlot = Cast<UVerticalBoxSlot>(Box_Choices->AddChild(ChoiceWidget)))
		{
			VSlot->SetHorizontalAlignment(HAlign_Fill);
			VSlot->SetPadding(FMargin(0.f, 4.f));
		}
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
