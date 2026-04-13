#include "Guest/UI/SaveLoad/GuestLoadBoardWidget.h"
#include "Guest/UI/SaveLoad/GuestSaveSlotWidget.h"
#include "Guest/Core/GameInstance/GuestGameInstance.h"
#include "Guest/Save/GuestSaveSlotNames.h"

void UGuestLoadBoardWidget::OnSlotWidgetCreated(UGuestSaveSlotWidget* SlotWidget, int32 SlotIndex)
{
	(void)SlotIndex;
	if (!SlotWidget)
	{
		return;
	}
	SlotWidget->OnSlotClicked.AddDynamic(this, &UGuestLoadBoardWidget::HandleLoadSlotClicked);
}

void UGuestLoadBoardWidget::HandleLoadSlotClicked(int32 InSlotIndex)
{
	UGuestGameInstance* GI = Cast<UGuestGameInstance>(GetGameInstance());
	if (!GI)
	{
		return;
	}
	const FString SlotName = GuestSaveSlots::MakeSlotName(InSlotIndex);
	GI->RequestLoadFromSlot(SlotName, GuestSaveSlots::DefaultUserIndex());
}