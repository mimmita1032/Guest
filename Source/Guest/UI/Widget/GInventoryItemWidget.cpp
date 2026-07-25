// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventoryItemWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/Texture2D.h"
#include "GItemDragDropOperation.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Sound/GuestSoundSubsystem.h"
#include "Guest/Sound/GuestSoundTags.h"

void UGInventoryItemWidget::InitItem(FInventoryItemHandle InHandle, TSoftObjectPtr<UTexture2D> InIcon,
                                     FIntPoint InGridSize, float InSlotSize, UTexture2D* InRuntimeIcon)
{
	ItemHandle        = InHandle;
	CachedIcon        = InIcon;
	CachedGridSize    = InGridSize;
	CachedSlotSize    = InSlotSize;
	CachedRuntimeIcon = InRuntimeIcon;

	if (Img_ItemIcon)
	{
		// 개체별 아이콘이 우선 — 사진은 찍힌 장면이 그대로 아이콘이 된다
		if (CachedRuntimeIcon)
		{
			Img_ItemIcon->SetBrushFromTexture(CachedRuntimeIcon);
		}
		else if (UTexture2D* Icon = CachedIcon.LoadSynchronous())
		{
			Img_ItemIcon->SetBrushFromTexture(Icon);
		}
	}
}

FReply UGInventoryItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (bInteractionLocked) return Reply;

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (ItemHandle.IsValid())
		{
			OnItemRightClicked.Broadcast(ItemHandle, InMouseEvent.GetScreenSpacePosition());
		}
		return Reply.Handled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return Reply;
}

void UGInventoryItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!ItemHandle.IsValid() || bInteractionLocked) return;

	// 사운드: 아이템을 클릭해서 집어들 때 (드래그 시작)
	if (UGuestSoundSubsystem* SoundSys = GetOwningPlayer()->GetGameInstance()->GetSubsystem<UGuestSoundSubsystem>())
	{
		SoundSys->PlayGlobalSound(GuestSoundTags::TAG_Sound_Event_UI_ButtonClick, AudioDataAsset);
	}

	UGInventoryItemWidget* DragVisual = CreateWidget<UGInventoryItemWidget>(GetOwningPlayer(), GetClass());
	if (!DragVisual) return;

	DragVisual->InitItem(ItemHandle, CachedIcon, CachedGridSize, CachedSlotSize, CachedRuntimeIcon);

	if (USizeBox* VisualRoot = Cast<USizeBox>(DragVisual->GetRootWidget()))
	{
		VisualRoot->SetWidthOverride(CachedGridSize.X * CachedSlotSize);
		VisualRoot->SetHeightOverride(CachedGridSize.Y * CachedSlotSize);
	}

	UGItemDragDropOperation* DragOp = NewObject<UGItemDragDropOperation>();
	DragOp->DraggedHandle     = ItemHandle;
	DragOp->DefaultDragVisual = DragVisual;
	DragOp->Pivot             = EDragPivot::MouseDown;

	const FVector2D LocalMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	DragOp->DragOffset = FIntPoint(
		FMath::FloorToInt(LocalMousePos.X / CachedSlotSize),
		FMath::FloorToInt(LocalMousePos.Y / CachedSlotSize)
	);

	OutOperation = DragOp;

	SetRenderOpacity(0.3f);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	G_LOG(TEXT("아이템 드래그 시작"));
}

void UGInventoryItemWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	SetRenderOpacity(1.0f);
	SetVisibility(ESlateVisibility::Visible);

	if (UGItemDragDropOperation* DragOp = Cast<UGItemDragDropOperation>(InOperation))
	{
		if (DragOp->DraggedHandle.IsValid())
		{
			// InventoryComponent에 직접 접근하지 않고 델리게이트로 위임
			// InventoryWidget이 구독해서 DropItem 호출
			OnItemDroppedOutside.Broadcast(DragOp->DraggedHandle);
			G_LOG(TEXT("인벤토리 외부 드롭: OnItemDroppedOutside Broadcast"));
		}
	}
}