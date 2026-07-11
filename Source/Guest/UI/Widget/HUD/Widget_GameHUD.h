// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h"
#include "Widget_GameHUD.generated.h"

class UGuestPawnUIComponent;

UCLASS(Abstract)
class GUEST_API UWidget_GameHUD : public UGuestActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

protected:
	// WBP에서 Progress Bar 등을 갱신 — BlueprintImplementableEvent는 protected 이상이어야 함
	UFUNCTION(BlueprintImplementableEvent, Category = "UI|Stat")
	void BP_OnHealthChanged(float Current, float Max);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI|Stat")
	void BP_OnBatteryChanged(float Current, float Max);

private:
	// PawnUIComponent Delegate 핸들러
	UFUNCTION()
	void HandleHealthChanged(float Current, float Max);

	UFUNCTION()
	void HandleBatteryChanged(float Current, float Max);
};
