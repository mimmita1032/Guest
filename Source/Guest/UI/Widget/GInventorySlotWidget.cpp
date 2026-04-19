// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventorySlotWidget.h"
#include "GItemDragDropOperation.h"
#include "Components/Image.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "Guest/Utils/GLog.h"

void UGInventorySlotWidget::SetItemReference(UGItemInstance* NewItem)
{
	ItemReference = NewItem;

	if (!Img_Icon) return;

	if (ItemReference)
	{
		if (const UGItemDefinition* ItemData = ItemReference->ItemDef) 
		{
			if (const UGItemFragmentInventory* InvFrag = ItemData->FindFragmentByClass<UGItemFragmentInventory>())
			{
				if (UTexture2D* IconTexture = InvFrag->ItemIcon.LoadSynchronous())
				{
					Img_Icon->SetBrushFromTexture(IconTexture);
					Img_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					return;
				}
			}
		}
	}

	Img_Icon->SetVisibility(ESlateVisibility::Hidden);
}

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
		if (UGItemInstance* DraggedItem = DragOp->DraggedItem)
		{
			if (APawn* OwningPawn = GetOwningPlayerPawn())
			{
				if (UGInventoryComponent* InvComp = OwningPawn->FindComponentByClass<UGInventoryComponent>())
				{
					// 컴포넌트의 실제 이동 함수 호출 예정
				}
			}
			if (APawn* OwningPawn = GetOwningPlayerPawn())
			{
				if (UGInventoryComponent* InvComp = OwningPawn->FindComponentByClass<UGInventoryComponent>())
				{
					InvComp->MoveItem(DraggedItem, SlotX, SlotY);
				}
			}
			G_LOG(TEXT("아이템 드롭 성공! 이동할 타겟 좌표 - X(열): %d, Y(행): %d"), SlotX, SlotY);
			
			return true;
		}
	}
	
	return false;
}