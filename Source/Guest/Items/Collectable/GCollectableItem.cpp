// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GCollectableItem.h"
#include "Guest/Data/DataAssets/GItemData.h"
#include "Guest/Utils/GLog.h"


AGCollectableItem::AGCollectableItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGCollectableItem::Interact(AActor* Interactor)
{
	Super::Interact(Interactor);
	
	if (GetItemData()) return;
	
	G_LOG(TEXT("수집형 아이템 [%s]이(가) 인벤토리에 추가되었습니다. (주체: %s"), *GetItemData()->ItemName.ToString(), *Interactor->GetName());
	
	// [TODO] 인벤토리에 아이템 추가
	
	// 임시 삭제 
	Destroy();
}

