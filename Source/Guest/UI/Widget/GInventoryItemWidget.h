// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "GInventoryItemWidget.generated.h"

class UImage;
class UTexture2D;
class UGuestAudioDataAsset; 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemDroppedOutside, FInventoryItemHandle, Handle);

UCLASS(Abstract)
class GUEST_API UGInventoryItemWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void InitItem(FInventoryItemHandle InHandle, TSoftObjectPtr<UTexture2D> InIcon,
				  FIntPoint InGridSize, float InSlotSize);

	UPROPERTY(BlueprintAssignable)
	FOnItemDroppedOutside OnItemDroppedOutside;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_ItemIcon;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Item")
	FInventoryItemHandle ItemHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Item")
	FIntPoint CachedGridSize = FIntPoint(1, 1);

	// Wwise 사운드 데이터 에셋
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Audio")
	TObjectPtr<UGuestAudioDataAsset> AudioDataAsset;

private:
	UPROPERTY()
	TSoftObjectPtr<UTexture2D> CachedIcon;

	float CachedSlotSize = 60.0f;
};