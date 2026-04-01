// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GItemFragment.h"
#include "GItemFragmentInventory.generated.h"

/*=================
인벤토리 관련정보
=================*/
UCLASS()
class GUEST_API UGItemFragmentInventory : public UGItemFragment
{
	GENERATED_BODY()
	
public:
	//인벤토리에서 보여질 아이콘
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSoftObjectPtr<UTexture2D> ItemIcon;
	
	//아이템 인벤토리 점유 크기
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	FIntPoint GridSize = FIntPoint(1,1);
};
