// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GInventoryWidget.h"
#include "GInventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Utils/GLog.h"
#include "Components/UniformGridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "GInventoryItemWidget.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"

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

	const int32 Columns = InventoryComponent->Columns;
	const int32 Rows = InventoryComponent->Rows;
	const int32 TotalSlots = Columns * Rows;
	
	// 슬롯 단일 픽셀 기준 크기
	const float SlotSize = 60.0f; 

	TSet<UGItemInstance*> ProcessedItems;

	//빈 배경 격자 생성
	for (int32 i = 0; i < TotalSlots; ++i)
	{
		if (UGInventorySlotWidget* NewSlot = CreateWidget<UGInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass))
		{
			int32 Row = i / Columns;
			int32 Col = i % Columns;

			NewSlot->SetSlotPosition(Col, Row);
			
			if (UUniformGridSlot* GridSlot = Grid_Inventory->AddChildToUniformGrid(NewSlot, Row, Col))
			{
				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);
			}

			NewSlot->SetItemReference(nullptr); 
		}
	}
	
	//캔버스 기반 아이템 배치
	for (int32 i = 0; i < TotalSlots; ++i)
	{
		int32 Row = i / Columns;
		int32 Col = i % Columns;

		if (UGItemInstance* ItemInst = InventoryComponent->GetItemAt(Col, Row))
		{
			if (ProcessedItems.Contains(ItemInst)) continue;
			ProcessedItems.Add(ItemInst);

			if (UGInventoryItemWidget* ItemWidget = CreateWidget<UGInventoryItemWidget>(GetOwningPlayer(), ItemWidgetClass))
			{
				ItemWidget->SetItemData(ItemInst);

				if (UCanvasPanelSlot* CanvasSlot = Canvas_Items->AddChildToCanvas(ItemWidget))
				{
					CanvasSlot->SetPosition(FVector2D(Col * SlotSize, Row * SlotSize));
					
					if (const UGItemDefinition* ItemData = ItemInst->ItemDef)
					{
						if (const UGItemFragmentInventory* InvFrag = ItemData->FindFragmentByClass<UGItemFragmentInventory>())
						{
							CanvasSlot->SetAutoSize(false);
							CanvasSlot->SetSize(FVector2D(InvFrag->GridSize.X * SlotSize, InvFrag->GridSize.Y * SlotSize));
						}
					}
				}
			}
		}
	}
	G_LOG(TEXT("인벤토리 갱신 완료: 총 %d칸, 아이템 %d개 배치"), TotalSlots, ProcessedItems.Num());
}