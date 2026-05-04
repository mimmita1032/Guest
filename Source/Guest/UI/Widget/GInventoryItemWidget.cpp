// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventoryItemWidget.h"
#include "Components/Image.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "GItemDragDropOperation.h"
#include "Components/SizeBox.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "GItemDragDropOperation.h"
#include "Guest/Utils/GLog.h"

void UGInventoryItemWidget::SetItemData(UGItemInstance* InItem)
{
	ItemReference = InItem;
	if (!Img_ItemIcon || !ItemReference) return;

	if (const UGItemDefinition* ItemData = ItemReference->GetItemDef())
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

	if (!ItemReference || !Img_ItemIcon) return;

	// 프래그먼트에서 GridSize 데이터 미리 추출
	FIntPoint ItemGridSize = FIntPoint(1, 1); // 기본 크기
	if (const UGItemDefinition* ItemData = ItemReference->GetItemDef())
	{
		if (const UGItemFragmentInventory* InvFrag = ItemData->FindFragmentByClass<UGItemFragmentInventory>())
		{
			ItemGridSize = InvFrag->GridSize;
		}
	}

	//마우스에 따라다닐 복제본 위젯
	UGInventoryItemWidget* DragVisual = CreateWidget<UGInventoryItemWidget>(GetOwningPlayer(), GetClass());
	if (DragVisual)
	{
		//복제본에 똑같은 사과 데이터를 주입하여 이미지를 띄움
		DragVisual->SetItemData(ItemReference);


		const float SlotSize = 60.0f;
		

		if (USizeBox* VisualRoot = Cast<USizeBox>(DragVisual->GetRootWidget()))
		{
			VisualRoot->SetWidthOverride(ItemGridSize.X * SlotSize);
			VisualRoot->SetHeightOverride(ItemGridSize.Y * SlotSize);
		}

		UGItemDragDropOperation* DragOp = NewObject<UGItemDragDropOperation>();
		DragOp->DraggedItem = ItemReference;
		DragOp->DefaultDragVisual = DragVisual;
		
		DragOp->Pivot = EDragPivot::MouseDown;

		FVector2D LocalMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		DragOp->DragOffset = FIntPoint(
			FMath::FloorToInt(LocalMousePos.X / SlotSize),
			FMath::FloorToInt(LocalMousePos.Y / SlotSize)
		);
		
		OutOperation = DragOp;

		//원본 반투명하게
		SetRenderOpacity(0.3f);
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		
		G_LOG(TEXT("아이템 드래그 시작: GridSize 데이터 기반 5x5 실제 크기가 반영된 잔상 생성 및 마우스 부착 완료"));
	}
}

void UGInventoryItemWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	//드래그 취소 시 원본 아이템의 시각적 상태 우선 복구
	SetRenderOpacity(1.0f);
	SetVisibility(ESlateVisibility::Visible);
	
	if (UGItemDragDropOperation* DragOp = Cast<UGItemDragDropOperation>(InOperation))
	{
		if (UGItemInstance* DraggedItem = DragOp->DraggedItem)
		{
			if (APawn* OwningPawn = GetOwningPlayerPawn())
			{
				if (UGInventoryComponent* InvComp = OwningPawn->FindComponentByClass<UGInventoryComponent>())
				{
					InvComp->DropItem(DraggedItem);
					G_LOG(TEXT("인벤토리 외부 드롭 감지: 아이템 월드 버리기(DropItem) 명령 컴포넌트로 전달 완료"));
				}
			}
		}
	}
}