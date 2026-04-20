// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GInventorySlotWidget.generated.h"

class UImage;
class UGItemInstance;

UCLASS(Abstract)
class GUEST_API UGInventorySlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	// 슬롯에 데이터를 주입하고 UI를 즉시 갱신
	void SetItemReference(UGItemInstance* NewItem);

protected:
	// 에디터의 'Img_Icon' 위젯과 C++ 변수를 강제 연결
	// 이 이름의 위젯이 WBP에 없으면 컴파일 에러가 발생하여 안전함
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Slot")
	TObjectPtr<UGItemInstance> ItemReference;

public:
	void SetSlotPosition(int32 InX, int32 InY);

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	int32 SlotX = 0;
	int32 SlotY = 0;
};