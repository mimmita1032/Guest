// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "Widget_GuestBase.h"
#include "Guest/Utils/GLog.h"

void UWidget_GuestBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWidget_GuestBase::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	G_LOG(TEXT("UI 활성화: %s"), *GetName());
}

void UWidget_GuestBase::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	G_LOG(TEXT("UI 비활성화: %s"), *GetName());
}