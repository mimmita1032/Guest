// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GInventoryWidget.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Utils/GLog.h"

void UGInventoryWidget::SetInventoryComponent(UGInventoryComponent* InComponent)
{
	if (InComponent)
	{
		InventoryComponent = InComponent;
		
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &UGInventoryWidget::OnRefreshInventory);
		
		UE_LOG(LogGSystem, Log, TEXT("위젯에 인벤토리 컴포넌트 연결 및 델리게이트 바인딩 완료~"));
	}
}

void UGInventoryWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

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