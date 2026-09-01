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

	if (!Node->TalkObjectiveID.IsNone())
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UGQuestSubsystem* QuestSys = GI->GetSubsystem<UGQuestSubsystem>())
			{
				QuestSys->OnObjectiveUpdated.Broadcast(Node->TalkObjectiveID, 1);
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

bool UGuestDialogueWidgetBase::EvaluateSingleCondition(const FString& InCondition) const
{
	const UWorld* World = GetWorld();
	const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	const UGQuestSubsystem* QuestSys = GI ? GI->GetSubsystem<UGQuestSubsystem>() : nullptr;
	if (!QuestSys) return false;

	FString Condition = InCondition.TrimStartAndEnd();

	// 앞의 "!" 는 부정. 조건을 하나 더 만들지 않고 뒤집어 쓴다.
	bool bNegate = false;
	if (Condition.StartsWith(TEXT("!")))
	{
		bNegate = true;
		Condition = Condition.RightChop(1).TrimStartAndEnd();
	}

	if (Condition.IsEmpty()) return true;

	bool bResult = false;
	FString QuestPart, StepPart;
	if (Condition.Split(TEXT("."), &QuestPart, &StepPart))
	{
		// "Completed" 는 단계 이름이 아니라 예약어다. 완료 여부를 묻는다.
		// 수락 선택지처럼 "그 퀘스트가 끝난 뒤에만 보여야 하는" 분기에 쓴다.
		if (StepPart.Equals(TEXT("Completed"), ESearchCase::IgnoreCase))
		{
			bResult = QuestSys->IsQuestCompleted(FName(*QuestPart));
		}
		else
		{
			bResult = QuestSys->GetCurrentStepID(FName(*QuestPart)) == FName(*StepPart);
		}
	}
	else
	{
		bResult = QuestSys->IsQuestActive(FName(*Condition));
	}

	return bNegate ? !bResult : bResult;
}

bool UGuestDialogueWidgetBase::IsChoiceConditionMet(FName ConditionID) const
{
	// 조건이 없으면 항상 표시
	if (ConditionID.IsNone()) return true;

	// "&" 로 이어진 조건은 전부 만족해야 한다.
	// 수락 선택지는 "1번 완료"와 "2번 미시작"을 동시에 요구하므로 하나로는 부족하다.
	TArray<FString> Parts;
	ConditionID.ToString().ParseIntoArray(Parts, TEXT("&"), true);
	if (Parts.Num() == 0) return true;

	for (const FString& Part : Parts)
	{
		if (!EvaluateSingleCondition(Part)) return false;
	}
	return true;
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
