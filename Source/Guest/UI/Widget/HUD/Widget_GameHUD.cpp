// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Widget_GameHUD.h"
#include "CommonUITypes.h"

void UWidget_GameHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
}

TOptional<FUIInputConfig> UWidget_GameHUD::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently, true);
}