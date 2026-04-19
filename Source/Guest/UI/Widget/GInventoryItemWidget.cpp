// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventoryItemWidget.h"
#include "Components/Image.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "GItemDragDropOperation.h"
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

	if (!ItemReference) return;

	//마우스에 따라다닐 복제본 위젯 생성
	UGInventoryItemWidget* DragVisual = CreateWidget<UGInventoryItemWidget>(GetOwningPlayer(), GetClass());
	if (DragVisual)
	{
		//복제본에 똑같은 사과 이미지
		DragVisual->SetItemData(ItemReference);

		UGItemDragDropOperation* DragOp = NewObject<UGItemDragDropOperation>();
		DragOp->DraggedItem = ItemReference;
		DragOp->DefaultDragVisual = DragVisual; //마우스 커서에 붙을 이미지
		
		//마우스를 클릭한 위치를 기준으로 이미지를 들고 다니도록 설정
		DragOp->Pivot = EDragPivot::MouseDown;

		OutOperation = DragOp;
		
		G_LOG(TEXT("아이템 드래그 시작: 오퍼레이션 객체 생성 및 마우스 부착 완료"));
	}
}