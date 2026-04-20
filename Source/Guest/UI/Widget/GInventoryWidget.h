// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GInventoryWidget.generated.h"

class UGInventoryComponent;
class UGInventorySlotWidget;
class UUniformGridPanel;
class UCanvasPanel;
class UGInventoryItemWidget;

UCLASS()
class GUEST_API UGInventoryWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void SetInventoryComponent(UGInventoryComponent* InComponent);

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeDestruct() override;
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	UFUNCTION()
	void OnRefreshInventory();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|UI")
	TObjectPtr<UGInventoryComponent> InventoryComponent;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> Grid_Inventory;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UGInventorySlotWidget> SlotWidgetClass;

	// 아이템 렌더링용 캔버스 패널
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Canvas_Items;

	// 아이템 전용 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UGInventoryItemWidget> ItemWidgetClass;
};
