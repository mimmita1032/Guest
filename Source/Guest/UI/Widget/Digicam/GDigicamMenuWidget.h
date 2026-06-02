// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Widget/GuestActivatableBase.h"
#include "GDigicamMenuWidget.generated.h"

class UWidgetSwitcher;

UCLASS()
class GUEST_API UGDigicamMenuWidget : public UGuestActivatableBase
{
	GENERATED_BODY()

public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	// 탭 버튼에서 BP로 호출
	UFUNCTION(BlueprintCallable, Category = "Digicam")
	void SwitchTab(int32 TabIndex);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> ContentSwitcher;
};
