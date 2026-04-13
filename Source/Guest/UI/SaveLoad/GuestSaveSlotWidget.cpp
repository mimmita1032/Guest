#include "Guest/UI/SaveLoad/GuestSaveSlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UGuestSaveSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (Btn_Slot)
	{
		Btn_Slot->OnClicked.AddDynamic(this, &UGuestSaveSlotWidget::HandleSlotButtonClicked);
	}
}

void UGuestSaveSlotWidget::HandleSlotButtonClicked()
{
	if (SlotIndex == INDEX_NONE)
	{
		return;
	}
	OnSlotClicked.Broadcast(SlotIndex);
}

void UGuestSaveSlotWidget::SetCreateAtDisplay(const FText& InText)
{
	if (Txt_CreateAt)
	{
		Txt_CreateAt->SetText(InText);
	}
	
}

void UGuestSaveSlotWidget::setMainQuestDisplay()
{
	if (Txt_CurrentMainQuest)
	{
		FString DisplayText = TEXT("진행중인 퀘스트");
		Txt_CurrentMainQuest->SetText(FText::FromString(DisplayText));
	}
}
