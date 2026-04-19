// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventoryItemWidget.h"
#include "Components/Image.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "Guest/Utils/GLog.h"

void UGInventoryItemWidget::SetItemData(UGItemInstance* InItem)
{
	ItemReference = InItem;
	if (!Img_ItemIcon || !ItemReference) return;

	if (const UGItemDefinition* ItemData = ItemReference->ItemDef)
	{
		if (const UGItemFragmentInventory* InvFrag = ItemData->FindFragmentByClass<UGItemFragmentInventory>())
		{
			if (UTexture2D* IconTexture = InvFrag->ItemIcon.LoadSynchronous())
			{
				Img_ItemIcon->SetBrushFromTexture(IconTexture);
			}
		}
	}
}

FReply UGInventoryItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return Reply;
}

void UGInventoryItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	G_LOG(TEXT("아이템 드래그 감지 성공. 다음 단계에서 DragDropOperation을 연결할 예정입니다."));
}