// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestPrimaryLayout.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/UI/GameplayTags/GuestGameplayTags.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UGuestPrimaryLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UGuestUISubsystem* UISubsystem = GI->GetSubsystem<UGuestUISubsystem>();
	if (!UISubsystem) return;

	// 각 스택을 태그와 함께 Subsystem 에 등록.
	// 이후 PushWidgetByTag 에서 태그로 스택을 찾아 위젯을 Push 함.
	UISubsystem->RegisterStack(GuestGameplayTags::TAG_WidgetStack_Frontend, Stack_Frontend);
	UISubsystem->RegisterStack(GuestGameplayTags::TAG_WidgetStack_GameHUD,  Stack_GameHUD);
	UISubsystem->RegisterStack(GuestGameplayTags::TAG_WidgetStack_GameMenu, Stack_GameMenu);
	UISubsystem->RegisterStack(GuestGameplayTags::TAG_WidgetStack_Modal,    Stack_Modal);

	// InputConfig 등록 (IMC 는 에디터에서 로드한 에셋 전달)
	UISubsystem->RegisterInputConfig(GuestGameplayTags::TAG_WidgetStack_Frontend, { EGuestInputMode::UIOnly,nullptr,1 });
	UISubsystem->RegisterInputConfig(GuestGameplayTags::TAG_WidgetStack_GameHUD,  { EGuestInputMode::GameOnly,nullptr, 0 });
	UISubsystem->RegisterInputConfig(GuestGameplayTags::TAG_WidgetStack_GameMenu, { EGuestInputMode::GameAndUI,nullptr,1 });
	UISubsystem->RegisterInputConfig(GuestGameplayTags::TAG_WidgetStack_Modal,    { EGuestInputMode::UIOnly,nullptr,2 });
}
