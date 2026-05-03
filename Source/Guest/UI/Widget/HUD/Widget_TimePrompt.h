// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Widget_TimePrompt.generated.h"

UCLASS(Abstract)
class GUEST_API UWidget_TimePrompt : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Guest|UI")
	void UpdateTimeText(const FText& NewTimeText);

private:
	
	UFUNCTION()
	void OnTimeChanged(float CurrentHour);
};