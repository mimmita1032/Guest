// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h" 
#include "GuestGameHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;

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
};