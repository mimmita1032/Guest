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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRightClicked, FInventoryItemHandle, Handle, FVector2D, ScreenPosition);

UCLASS(Abstract)
class GUEST_API UGInventoryItemWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	// InRuntimeIcon이 있으면 InIcon 대신 그것을 표시한다 (사진처럼 개체마다 그림이 다른 아이템)
	void InitItem(FInventoryItemHandle InHandle, TSoftObjectPtr<UTexture2D> InIcon,
				  FIntPoint InGridSize, float InSlotSize, UTexture2D* InRuntimeIcon = nullptr);

	UPROPERTY(BlueprintAssignable)
	FOnItemDroppedOutside OnItemDroppedOutside;

	// 우클릭 시 브로드캐스트 — GInventoryWidget이 받아 컨텍스트 메뉴 표시에 사용
	UPROPERTY(BlueprintAssignable)
	FOnItemRightClicked OnItemRightClicked;

	// 배치 모드 등 다른 상호작용이 진행 중일 때 드래그 시작을 막기 위한 잠금
	UFUNCTION(BlueprintCallable, Category = "Inventory|Item")
	void SetInteractionLocked(bool bLocked) { bInteractionLocked = bLocked; }

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

	// 개체별 아이콘 (드래그 비주얼 생성 시에도 그대로 이어가야 하므로 캐시한다)
	UPROPERTY()
	TObjectPtr<UTexture2D> CachedRuntimeIcon;

	float CachedSlotSize = 60.0f;

	bool bInteractionLocked = false;
};