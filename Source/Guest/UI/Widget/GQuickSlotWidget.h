// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GQuickSlotWidget.generated.h"

class UOverlay;
class UGInventoryItemWidget;
class UGuestAudioDataAsset;

UCLASS(Abstract)
class GUEST_API UGQuickSlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	// 이 퀵슬롯의 번호 (0 ~ 3)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|QuickSlot")
	int32 QuickSlotIndex = 0;

	// 장비 슬롯과 동일하게 오버레이 사용
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UOverlay> Overlay_ItemContainer;

	// 이미지 텍스처 대신 아이템 위젯을 통째로 받도록 변경
	void RefreshSlotUI(UGInventoryItemWidget* ItemWidget);

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Audio")
	TObjectPtr<UGuestAudioDataAsset> AudioDataAsset;
};