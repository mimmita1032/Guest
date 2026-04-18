// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GInventorySlotWidget.generated.h"

UCLASS(Abstract)
class GUEST_API UGInventorySlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Slot")
	void OnSlotUpdated();
};
