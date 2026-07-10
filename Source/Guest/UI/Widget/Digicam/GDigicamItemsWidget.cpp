// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GDigicamItemsWidget.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"

void UGDigicamItemsWidget::OnTabActivated_Implementation()
{
	if (UGuestUISubsystem* UISys = GetUISubsystem())
	{
		if (!UISys->IsWidgetActive(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_Inventory))
		{
			UISys->PushWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_Inventory);
		}
	}
}

UGuestUISubsystem* UGDigicamItemsWidget::GetUISubsystem() const
{
	const UGameInstance* GI = GetGameInstance();
	return GI ? GI->GetSubsystem<UGuestUISubsystem>() : nullptr;
}
