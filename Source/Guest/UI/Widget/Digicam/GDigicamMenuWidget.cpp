// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GDigicamMenuWidget.h"
#include "Components/WidgetSwitcher.h"

void UGDigicamMenuWidget::SwitchTab(int32 TabIndex)
{
	if (ContentSwitcher)
	{
		ContentSwitcher->SetActiveWidgetIndex(TabIndex);
	}
}

TOptional<FUIInputConfig> UGDigicamMenuWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::NoCapture);
}
