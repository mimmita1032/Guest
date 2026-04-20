#include "Guest/UI/SaveLoad/GuestSaveBoardWidget.h"
#include "Guest/UI/SaveLoad/GuestSaveSlotWidget.h"
#include "Guest/Core/Controllers/GuestPlayerController.h"
#include "Guest/Save/GuestSaveSlotNames.h"

void UGuestSaveBoardWidget::OnSlotWidgetCreated(UGuestSaveSlotWidget* SlotWidget, int32 SlotIndex)
{
	(void)SlotIndex;
	if (!SlotWidget)
	{
		return;
	}
	SlotWidget->OnSlotClicked.AddDynamic(this, &UGuestSaveBoardWidget::HandleSaveSlotClicked);
}

void UGuestSaveBoardWidget::HandleSaveSlotClicked(int32 InSlotIndex)
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}
	AGuestPlayerController* GuestPC = Cast<AGuestPlayerController>(PC);
	if (!GuestPC)
	{
		return;
	}
	
	const FString SlotName = GuestSaveSlots::MakeSlotName(InSlotIndex);
	const bool bSaved = GuestPC->SaveCurrentGameToSlot(SlotName, GuestSaveSlots::DefaultUserIndex());
	
	if (bSaved)
	{
		PopulateSlots();
	}
	
}
