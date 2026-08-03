// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GuestNarrationWidget.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"

void UGuestNarrationWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	UGuestUISubsystem* UISys = GetUISubsystem();
	if (!UISys) return;

	StartNarrationSession(UISys->GetPendingNarrationAsset());
}

void UGuestNarrationWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (UGuestUISubsystem* UISys = GetUISubsystem())
	{
		UISys->NotifyWidgetDeactivated(GuestGameplayTags::TAG_WidgetStack_Narration);
	}

	ResetNarrationSession();
}
