#include "GuestSaveSlotBoardBase.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"


void UGuestSaveSlotBoardBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (Btn_close)
	{
		Btn_close->OnClicked.AddDynamic(this, &UGuestSaveSlotBoardBase::OnCloseClicked);
	}
	
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
