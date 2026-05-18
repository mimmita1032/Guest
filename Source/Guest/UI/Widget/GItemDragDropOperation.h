// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "GItemDragDropOperation.generated.h"

UCLASS()
class GUEST_API UGItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	// 드래그 중인 아이템 핸들
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|DragDrop")
	FInventoryItemHandle DraggedHandle;

	// 아이템 좌상단 기준으로 몇 번째 셀을 클릭했는지
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|DragDrop")
	FIntPoint DragOffset;
};
