// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "GInventorySlotWidget.generated.h"

class UImage;
class UGuestAudioDataAsset;

// 슬롯에 아이템이 드롭됐을 때 소유 GInventoryWidget에 위임하기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSlotItemDropped, FInventoryItemHandle, Handle, int32, TargetX, int32, TargetY);

UCLASS(Abstract)
class GUEST_API UGInventorySlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetSlotPosition(int32 InX, int32 InY);

	UPROPERTY(BlueprintAssignable, Category = "Inventory|UI")
	FOnSlotItemDropped OnSlotItemDropped;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	// ★ Wwise 사운드 데이터 에셋
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Audio")
	TObjectPtr<UGuestAudioDataAsset> AudioDataAsset;

private:
	int32 SlotX = 0;
	int32 SlotY = 0;
};