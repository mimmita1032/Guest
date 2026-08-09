// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GQuickSlotWidget.h"
#include "GItemDragDropOperation.h"
#include "GInventoryItemWidget.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Sound/GuestSoundSubsystem.h"
#include "Guest/Sound/GuestSoundTags.h"
#include "Components/OverlaySlot.h"
#include "Components/Overlay.h"

void UGQuickSlotWidget::RefreshSlotUI(UGInventoryItemWidget* ItemWidget)
{
	if (Overlay_ItemContainer)
	{
		Overlay_ItemContainer->ClearChildren();
		if (ItemWidget)
		{
			UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Overlay_ItemContainer->AddChild(ItemWidget));
			if (OverlaySlot)
			{
				// 자식 위젯이 부모(장비 슬롯)의 크기에 꽉 차도록 확장
				OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
				OverlaySlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}
}

bool UGQuickSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UGItemDragDropOperation* DragOp = Cast<UGItemDragDropOperation>(InOperation);
	if (!DragOp || !DragOp->DraggedHandle.IsValid()) return false;

	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn) return false;

	UGInventoryComponent* InvComp = OwningPawn->FindComponentByClass<UGInventoryComponent>();
	if (!InvComp) return false;

	// 퀵슬롯 할당 실행
	if (InvComp->AssignQuickSlot(DragOp->DraggedHandle, QuickSlotIndex))
	{
		// 할당 사운드 재생
		if (UGuestSoundSubsystem* SoundSys = GetOwningPlayer()->GetGameInstance()->GetSubsystem<UGuestSoundSubsystem>())
		{
			SoundSys->PlayGlobalSound(GuestSoundTags::TAG_Sound_Event_UI_ButtonClick, AudioDataAsset);
		}
		return true;
	}

	return false;
}