// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "GInventoryWidget.generated.h"

class UGInventoryComponent;
class UGInventorySlotWidget;
class UUniformGridPanel;
class UCanvasPanel;
class UGInventoryItemWidget;
class UGuestAudioDataAsset;

UCLASS()
class GUEST_API UGInventoryWidget : public UGuestActivatableBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void SetInventoryComponent(UGInventoryComponent* InComponent);

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnRefreshInventory();

	// ItemWidget의 OnItemDroppedOutside 델리게이트 수신 → DropItem 처리
	UFUNCTION()
	void HandleItemDroppedOutside(FInventoryItemHandle Handle);

	// SlotWidget의 OnSlotItemDropped 델리게이트 수신 → MoveItem 처리
	UFUNCTION()
	void HandleSlotItemDropped(FInventoryItemHandle Handle, int32 TargetX, int32 TargetY);

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

	// SlotSize는 여기서만 관리 — ItemWidget에 InitItem으로 전달
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	float SlotSize = 60.0f;

	// ★ Wwise 사운드 데이터 에셋
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Audio")
	TObjectPtr<UGuestAudioDataAsset> AudioDataAsset;
};
