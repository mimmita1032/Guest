#include "GuestSaveSlotBoardBase.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Guest/UI/SaveLoad/GuestSaveSlotWidget.h"
#include "Kismet/GameplayStatics.h"


void UGuestSaveSlotBoardBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (Btn_close)
	{
		Btn_close->OnClicked.AddDynamic(this, &UGuestSaveSlotBoardBase::OnCloseClicked);
	}
	PopulateSlots();
	
}
void UGuestSaveSlotBoardBase::OnCloseClicked()
{
	RemoveFromParent();
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}
}

void UGuestSaveSlotBoardBase::PopulateSlots()
{
	if (!SB_SlotList || !SaveSlotClass)
	{
		return;
	}
	SB_SlotList->ClearChildren();
	const int32 Count = NumSlots; // 또는 4 상수
	for (int32 i = 0; i < Count; ++i)
	{
		UGuestSaveSlotWidget* SlotWidget  = CreateWidget<UGuestSaveSlotWidget>(this, SaveSlotClass);
		if (!SlotWidget )
		{
			continue;
		}
		SlotWidget ->SetSlotIndex(i);
		const FString SlotName = FString::Printf(TEXT("GuestSave_%d"), i);
		const bool bExists = UGameplayStatics::DoesSaveGameExist(SlotName, 0);
		if (bExists)
		{
			// LoadGameFromSlot 후 Cast<UGuestSaveGame> → SavedAt 포맷해서 SetCreateAtDisplay
		}
		else
		{
			SlotWidget ->SetCreateAtDisplay(NSLOCTEXT("Guest", "EmptySlot", "-"));
		}
		SB_SlotList->AddChild(SlotWidget);
	}
}
