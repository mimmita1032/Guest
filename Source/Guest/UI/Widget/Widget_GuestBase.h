// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Widget_GuestBase.generated.h"

UCLASS(Abstract)
class GUEST_API UWidget_GuestBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;
};