// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GInventoryWidget.h"
#include "GInventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Utils/GLog.h"
#include "Components/UniformGridSlot.h"

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
	if (!InventoryComponent || !Grid_Inventory || !SlotWidgetClass)
	{
		G_WARN(TEXT("인벤토리 갱신 실패: 컴포넌트, 그리드 패널, 또는 슬롯 클래스가 없습니다."));
		return;
	}

	Grid_Inventory->ClearChildren();

	const int32 Columns = InventoryComponent->Columns;
	const int32 Rows = InventoryComponent->Rows;
	const int32 TotalSlots = Columns * Rows;

	for (int32 i = 0; i < TotalSlots; ++i)
	{
		if (UGInventorySlotWidget* NewSlot = CreateWidget<UGInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass))
		{
			int32 Row = i / Columns;
			int32 Col = i % Columns;
          
			if (UUniformGridSlot* GridSlot = Grid_Inventory->AddChildToUniformGrid(NewSlot, Row, Col))
			{
				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);
			}

			if (UGItemInstance* ItemAtSlot = InventoryComponent->GetItemAt(Col, Row)) 
			{
				NewSlot->SetItemReference(ItemAtSlot);
			}
			else
			{
				NewSlot->SetItemReference(nullptr); 
			}
		}
	}
	G_LOG(TEXT("인벤토리 그리드 갱신 완료: 총 %d칸 생성"), TotalSlots);
}