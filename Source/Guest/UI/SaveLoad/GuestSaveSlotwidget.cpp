#include "GuestSaveSlotwidget.h"
#include "Components/TextBlock.h"

void UGuestSaveSlotWidget::SetCreateAtDisplay(const FText& InText)
{
	if (Txt_CreateAt)
	{
		Txt_CreateAt->SetText(InText);
	}
}
