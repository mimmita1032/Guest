// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/HUD/Frontend/GuestDemoEndWidget.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"
#include "Guest/UI/Base/GuestCommonButton.h"
#include "Kismet/KismetSystemLibrary.h"

void UGuestDemoEndWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_MainMenu) Button_MainMenu->OnClicked().AddUObject(this, &UGuestDemoEndWidget::OnMainMenuClicked);
	if (Button_Quit)     Button_Quit->OnClicked().AddUObject(this, &UGuestDemoEndWidget::OnQuitClicked);
}

void UGuestDemoEndWidget::OnMainMenuClicked()
{
	if (UGuestUISubsystem* UISys = GetUISubsystem())
	{
		UISys->PushWidget(
			GuestGameplayTags::TAG_WidgetStack_Frontend,
			GuestGameplayTags::TAG_Widget_MainMenu
		);
	}
}

void UGuestDemoEndWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(
		this,
		GetOwningPlayer(),
		EQuitPreference::Quit,
		false
	);
}
