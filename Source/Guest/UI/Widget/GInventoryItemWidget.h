// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GInventoryItemWidget.generated.h"

class UImage;
class UGItemInstance;

UCLASS(Abstract)
class GUEST_API UGInventoryItemWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetItemData(UGItemInstance* InItem);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_ItemIcon;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Item")
	TObjectPtr<UGItemInstance> ItemReference;
};