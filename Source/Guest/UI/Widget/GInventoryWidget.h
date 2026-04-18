// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GInventoryWidget.generated.h"

class UGInventoryComponent;

UCLASS()
class GUEST_API UGInventoryWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void SetInventoryComponent(UGInventoryComponent* InComponent);

protected:
	virtual void NativeOnActivated() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|UI")
	void OnRefreshInventory();

	virtual void NativeDestruct() override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|UI")
	TObjectPtr<UGInventoryComponent> InventoryComponent;
};
