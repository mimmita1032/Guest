// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GuestNPCDialogueWidget.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"

void UGuestNPCDialogueWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	UGuestUISubsystem* UISys = GetUISubsystem();
	if (!UISys) return;

	StartDialogueSession(UISys->GetPendingDialogueAsset());
}

void UGuestNPCDialogueWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	ResetDialogueSession();
}
