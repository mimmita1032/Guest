// Copyright (c) 2026 Anything Left Behind?. 사전에 약속된 저작권 문구. All rights reserved.

#include "GInventorySlotWidget.h"
#include "GItemDragDropOperation.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Sound/GuestSoundSubsystem.h"
#include "Guest/Sound/GuestSoundTags.h"

void UGInventorySlotWidget::SetSlotPosition(int32 InX, int32 InY)
{
	SlotX = InX;
	SlotY = InY;
}

bool UGInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (UGItemDragDropOperation* DragOp = Cast<UGItemDragDropOperation>(InOperation))
	{
		if (DragOp->DraggedHandle.IsValid())
		{
			const int32 TargetX = SlotX - DragOp->DragOffset.X;
			const int32 TargetY = SlotY - DragOp->DragOffset.Y;

			OnSlotItemDropped.Broadcast(DragOp->DraggedHandle, TargetX, TargetY);

			// 사운드: 슬롯에 아이템을 내려놓았을 때 (장착/드롭 사운드)
			if (UGuestSoundSubsystem* SoundSys = GetOwningPlayer()->GetGameInstance()->GetSubsystem<UGuestSoundSubsystem>())
			{
				SoundSys->PlayGlobalSound(GuestSoundTags::TAG_Sound_Event_UI_ButtonClick, AudioDataAsset);
			}

			G_LOG(TEXT("아이템 드롭: 타겟 좌표 (%d, %d)"), TargetX, TargetY);
			return true;
		}
	}

	return false;
}