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
class UGEquipSlotWidget;
class UGQuickSlotWidget;

UCLASS()
class GUEST_API UGInventoryWidget : public UGuestActivatableBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void SetInventoryComponent(UGInventoryComponent* InComponent);

	// 컨텍스트 메뉴의 "놓기" 클릭 시 BP에서 호출 — 배치 모드를 시작시킴
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void RequestPlaceItem(FInventoryItemHandle Handle);

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeDestruct() override;
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void OnRefreshInventory();

	// ItemWidget의 OnItemDroppedOutside 델리게이트 수신 → DropItem 처리
	UFUNCTION()
	void HandleItemDroppedOutside(FInventoryItemHandle Handle);

	// SlotWidget의 OnSlotItemDropped 델리게이트 수신 → MoveItem 처리
	UFUNCTION()
	void HandleSlotItemDropped(FInventoryItemHandle Handle, int32 TargetX, int32 TargetY);

	// ItemWidget의 OnItemRightClicked 델리게이트 수신 → 컨텍스트 메뉴 표시 요청
	UFUNCTION()
	void HandleItemRightClicked(FInventoryItemHandle Handle, FVector2D ScreenPosition);

	// UGItemPlacementComponent::OnPlacementStateChanged 수신 → 인벤토리 반투명화/드래그 잠금
	UFUNCTION()
	void HandlePlacementStateChanged(bool bActive);

	// 컨텍스트 메뉴 표시는 BP에서 구현 (위치·항목 구성은 UMG 쪽 재량)
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|UI")
	void BP_ShowItemContextMenu(FInventoryItemHandle Handle, FVector2D ScreenPosition);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|UI")
	TObjectPtr<UGInventoryComponent> InventoryComponent;

	// ─── 그리드 인벤토리 패널 ───
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> Grid_Inventory;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Canvas_Items;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UGInventorySlotWidget> SlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UGInventoryItemWidget> ItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	float SlotSize = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Audio")
	TObjectPtr<UGuestAudioDataAsset> AudioDataAsset;

	// ─── 장비 슬롯 ───
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UGEquipSlotWidget> EquipSlot_Helmet;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UGEquipSlotWidget> EquipSlot_Chest;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UGEquipSlotWidget> EquipSlot_Legs;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UGEquipSlotWidget> EquipSlot_Boots;

	// ─── 퀵슬롯 ───
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UGQuickSlotWidget> QuickSlot_0;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UGQuickSlotWidget> QuickSlot_1;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UGQuickSlotWidget> QuickSlot_2;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UGQuickSlotWidget> QuickSlot_3;

private:
	UPROPERTY()
	TArray<TObjectPtr<UGInventoryItemWidget>> SpawnedItemWidgets;

	bool bPlacementModeActive = false;
};