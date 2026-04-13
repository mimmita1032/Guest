#pragma once
#include "CoreMinimal.h"
#include "Guest/UI/SaveLoad/GuestSaveSlotBoardBase.h"
#include "GuestLoadBoardWidget.generated.h"

class UGuestSaveSlotWidget;

UCLASS()
class GUEST_API UGuestLoadBoardWidget : public UGuestSaveSlotBoardBase
{
	GENERATED_BODY()
	
protected:
	virtual void OnSlotWidgetCreated(UGuestSaveSlotWidget* SlotWidget, int32 SlotIndex) override;
	UFUNCTION()
	void HandleLoadSlotClicked(int32 SlotIndex);
};