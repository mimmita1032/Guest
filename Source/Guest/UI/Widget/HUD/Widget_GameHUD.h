// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Widget/GuestActivatableBase.h"
#include "Widget_GameHUD.generated.h"

UCLASS(Abstract)
class GUEST_API UWidget_GameHUD : public UGuestActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
};