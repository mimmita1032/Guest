// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventorySlotWidget.h"
#include "GItemDragDropOperation.h"
#include "Guest/Utils/GLog.h"

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
			G_LOG(TEXT("아이템 드롭: 타겟 좌표 (%d, %d)"), TargetX, TargetY);
			return true;
		}
	}

	return false;
}
