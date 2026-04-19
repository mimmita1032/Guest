// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "GItemDragDropOperation.generated.h"

class UGItemInstance;

UCLASS()
class GUEST_API UGItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	// 마우스가 현재 운반 중인 아이템 데이터 (드랍할 때 꺼내볼 용도)
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|DragDrop")
	TObjectPtr<UGItemInstance> DraggedItem;

	// 클릭한 위치(아이템 좌상단 기준 몇 번째 칸을 잡았는지)
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|DragDrop")
	FIntPoint DragOffset;
};