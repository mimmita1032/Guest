#pragma once
#include "CoreMinimal.h"
#include "Guest/UI/SaveLoad/GuestSaveSlotBoardBase.h"
#include "GuestSaveBoardWidget.generated.h"


UCLASS()
class GUEST_API UGuestSaveBoardWidget:public UGuestSaveSlotBoardBase
{
	
	GENERATED_BODY()
	
protected:
	virtual void OnSlotWidgetCreated(UGuestSaveSlotWidget* SlotWidget, int32 SlotIndex) override;
	UFUNCTION()
	void HandleSaveSlotClicked(int32 SlotIndex);
};
