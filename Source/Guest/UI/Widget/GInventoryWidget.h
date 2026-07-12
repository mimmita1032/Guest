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

private:
	// 배치 모드 토글 시 잠금 상태를 일괄 갱신하기 위해 보관 (OnRefreshInventory마다 갱신)
	UPROPERTY()
	TArray<TObjectPtr<UGInventoryItemWidget>> SpawnedItemWidgets;

	bool bPlacementModeActive = false;
};
