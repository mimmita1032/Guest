// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventoryWidget.h"
#include "GInventorySlotWidget.h"
#include "GInventoryItemWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Utils/GLog.h"

void UGInventoryWidget::SetInventoryComponent(UGInventoryComponent* InComponent)
{
	if (InComponent)
	{
		InventoryComponent = InComponent;
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &UGInventoryWidget::OnRefreshInventory);
		G_LOG(TEXT("인벤토리 위젯: 컴포넌트 연결 및 델리게이트 바인딩 완료"));
	}
}

void UGInventoryWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (!InventoryComponent)
	{
		if (APawn* OwningPawn = GetOwningPlayerPawn())
		{
			if (UGInventoryComponent* InvComp = OwningPawn->FindComponentByClass<UGInventoryComponent>())
			{
				SetInventoryComponent(InvComp);
			}
		}
	}

	OnRefreshInventory();
}

void UGInventoryWidget::NativeDestruct()
{
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveAll(this);
	}
	Super::NativeDestruct();
}

TOptional<FUIInputConfig> UGInventoryWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::NoCapture);
}

void UGInventoryWidget::OnRefreshInventory()
{
	if (!InventoryComponent || !Grid_Inventory || !SlotWidgetClass || !Canvas_Items || !ItemWidgetClass)
	{
		G_WARN(TEXT("인벤토리 갱신 실패: 패널이나 클래스 설정이 누락되었습니다."));
		return;
	}

	Grid_Inventory->ClearChildren();
	Canvas_Items->ClearChildren();

	const int32 Columns   = InventoryComponent->Columns;
	const int32 Rows      = InventoryComponent->Rows;
	const int32 TotalSlots = Columns * Rows;

	// 빈 배경 격자 생성
	for (int32 i = 0; i < TotalSlots; ++i)
	{
		if (UGInventorySlotWidget* NewSlot = CreateWidget<UGInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass))
		{
			const int32 Row = i / Columns;
			const int32 Col = i % Columns;

			NewSlot->SetSlotPosition(Col, Row);

			if (UUniformGridSlot* GridSlot = Grid_Inventory->AddChildToUniformGrid(NewSlot, Row, Col))
			{
				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}

	// 핸들 목록 한 번만 조회해 재사용
	const TArray<FInventoryItemHandle> Handles = InventoryComponent->GetAllHandles();

	for (const FInventoryItemHandle& Handle : Handles)
	{
		const FInventoryItemRenderData RenderData = InventoryComponent->GetItemRenderData(Handle);
		if (!RenderData.bIsValid) continue;
		if (RenderData.Position.X < 0 || RenderData.Position.Y < 0) continue;

		if (UGInventoryItemWidget* ItemWidget = CreateWidget<UGInventoryItemWidget>(GetOwningPlayer(), ItemWidgetClass))
		{
			ItemWidget->InitItem(Handle, RenderData.Icon, RenderData.GridSize, SlotSize);

			// OnRefreshInventory 호출마다 위젯을 새로 생성하므로 중복 바인딩 없음
			ItemWidget->OnItemDroppedOutside.AddDynamic(this, &UGInventoryWidget::HandleItemDroppedOutside);

			if (UCanvasPanelSlot* CanvasSlot = Canvas_Items->AddChildToCanvas(ItemWidget))
			{
				CanvasSlot->SetAutoSize(false);
				CanvasSlot->SetPosition(FVector2D(RenderData.Position.X * SlotSize, RenderData.Position.Y * SlotSize));
				CanvasSlot->SetSize(FVector2D(RenderData.GridSize.X * SlotSize, RenderData.GridSize.Y * SlotSize));
			}
		}
	}

	G_LOG(TEXT("인벤토리 갱신 완료: 총 %d칸, 아이템 %d개 배치"), TotalSlots, Handles.Num());
}

void UGInventoryWidget::HandleItemDroppedOutside(FInventoryItemHandle Handle)
{
	if (!Handle.IsValid()) return;
	if (!InventoryComponent) return;
	InventoryComponent->DropItem(Handle);
}