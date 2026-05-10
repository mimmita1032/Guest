// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GInventorySlotWidget.generated.h"

class UImage;

UCLASS(Abstract)
class GUEST_API UGInventorySlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetSlotPosition(int32 InX, int32 InY);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	int32 SlotX = 0;
	int32 SlotY = 0;
};
