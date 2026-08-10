// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "GameplayTagContainer.h"
#include "GEquipSlotWidget.generated.h"

class UOverlay;
class UGInventoryItemWidget;
class UGuestAudioDataAsset;

UCLASS(Abstract)
class GUEST_API UGEquipSlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	// 이 슬롯이 담당할 장비 부위 (블루프린트 디테일 패널에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Equipment")
	EEquipSlot EquipSlotType = EEquipSlot::None;

	// 이 슬롯에 장착을 허용할 아이템 태그 (예: Item.Equip.Helmet)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Equipment")
	FGameplayTag AllowedItemTag;

	// 슬롯에 아이템 위젯을 자식으로 부착할 오버레이 (UMG에서 동일한 이름으로 추가)
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UOverlay> Overlay_ItemContainer;

	// 장착된 아이템 위젯을 화면에 갱신하는 함수
	void RefreshSlotUI(UGInventoryItemWidget* ItemWidget);

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Audio")
	TObjectPtr<UGuestAudioDataAsset> AudioDataAsset;
};