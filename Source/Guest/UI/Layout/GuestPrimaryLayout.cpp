// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GuestPrimaryLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/UI/GameplayTags/GuestGameplayTags.h"

void UGuestPrimaryLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGuestUISubsystem* UISubsystem = GI->GetSubsystem<UGuestUISubsystem>())
		{
			// 각 스택을 태그와 함께 Subsystem 에 등록.
			UISubsystem->RegisterStack(GuestGameplayTags::TAG_WidgetStack_Frontend, Stack_Frontend);
			UISubsystem->RegisterStack(GuestGameplayTags::TAG_WidgetStack_GameHUD, Stack_GameHUD);
			UISubsystem->RegisterStack(GuestGameplayTags::TAG_WidgetStack_GameMenu, Stack_GameMenu);
			UISubsystem->RegisterStack(GuestGameplayTags::TAG_WidgetStack_Modal, Stack_Modal);
		}
	}
}