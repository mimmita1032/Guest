#pragma once
#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h"
#include "GuestSaveSlotBoardBase.generated.h"

class UGuestSaveSlotWidget;

UCLASS()
class GUEST_API UGuestSaveSlotBoardBase : public UGuestActivatableBase
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScrollBox> SB_SlotList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_close;

	UPROPERTY(EditDefaultsOnly, Category = "Save")
	TSubclassOf<class UGuestSaveSlotWidget> SaveSlotClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Save")
	int32 NumSlots = 4;
	
	virtual void OnSlotWidgetCreated(UGuestSaveSlotWidget* SlotWidget, int32 SlotIndex) {}
	
	// --- 버튼 이벤트 ---
	UFUNCTION()
	void OnCloseClicked();
	
	void PopulateSlots();
};
