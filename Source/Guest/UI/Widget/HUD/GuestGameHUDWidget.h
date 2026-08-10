// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h" 
#include "GuestGameHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

UCLASS()
class GUEST_API UGuestGameHUDWidget : public UGuestActivatableBase 
{
	GENERATED_BODY()

public:
	// 체력 UI 업데이트 함수
	UFUNCTION(BlueprintCallable, Category = "Guest|UI")
	void UpdateHealthUI(float CurrentHealth, float MaxHealth);

	// 배터리(에너지) UI 업데이트 함수
	UFUNCTION(BlueprintCallable, Category = "Guest|UI")
	void UpdateBatteryUI(float CurrentBattery, float MaxBattery);

	// 인벤토리 퀵슬롯 UI 갱신 함수
	UFUNCTION()
	void UpdateQuickSlotsUI();

protected:
	virtual void NativeConstruct() override;
    
protected:
	// --- UI 컴포넌트 바인딩 ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Health;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Health;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Battery;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Battery;

	// --- 퀵슬롯 이미지 바인딩 (BindWidgetOptional로 설정하여 에디터 작업 전 크래시 방지) ---
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Img_QuickSlot_0;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Img_QuickSlot_1;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Img_QuickSlot_2;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Img_QuickSlot_3;
};