// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GDigicamMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "GDigicamTabBase.h"

void UGDigicamMenuWidget::SwitchTab(int32 TabIndex)
{
	if (!ContentSwitcher) return;

	if (UGDigicamTabBase* OldTab = Cast<UGDigicamTabBase>(ContentSwitcher->GetActiveWidget()))
	{
		OldTab->OnTabDeactivated();
	}

	ContentSwitcher->SetActiveWidgetIndex(TabIndex);

	if (UGDigicamTabBase* NewTab = Cast<UGDigicamTabBase>(ContentSwitcher->GetActiveWidget()))
	{
		NewTab->OnTabActivated();
	}
}

TOptional<FUIInputConfig> UGDigicamMenuWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::NoCapture);
}
