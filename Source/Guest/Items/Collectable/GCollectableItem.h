// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/Items/Base/GItemBase.h"
#include "GCollectableItem.generated.h"

UCLASS()
class GUEST_API AGCollectableItem : public AGItemBase
{
	GENERATED_BODY()

public:
	AGCollectableItem();
	
	// 수집 로직 구현
	virtual void Interact(AActor* Interactor) override;
};
