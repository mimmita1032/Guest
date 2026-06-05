// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/HUD/Modal/GuestOptionWidget.h"
#include "Guest/UI/Base/GuestCommonButton.h"
#include "Guest/UI/Base/GuestTabList.h" // 헤더 추가
#include "CommonAnimatedSwitcher.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"

void UGuestOptionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_Quit)
	{
		Button_Quit->OnClicked().AddUObject(this, &ThisClass::OnQuitClicked);
	}

	if (TabList_Menu && Switcher_Content)
	{
		TabList_Menu->SetLinkedSwitcher(Switcher_Content);
		TabList_Menu->OnTabSelected.AddDynamic(this, &ThisClass::OnTabSelected);
	}
}

void UGuestOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 위젯이 뷰포트에 완전히 그려질 준비가 된 시점에 탭 등록
	if (TabList_Menu && TabButtonClass && TabList_Menu->GetTabCount() == 0)
	{
		TabList_Menu->RegisterTab(FName("Tab_General"),  TabButtonClass, Panel_GamePlay);
		TabList_Menu->RegisterTab(FName("Tab_Graphics"), TabButtonClass, Panel_Graphics);
		TabList_Menu->RegisterTab(FName("Tab_Sound"),    TabButtonClass, Panel_Sound);
		TabList_Menu->RegisterTab(FName("Tab_Controls"), TabButtonClass, Panel_Controls);
	}
}

void UGuestOptionWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (TabList_Menu)
	{
		TabList_Menu->SelectTabByID(FName("Tab_General"));
	}
}

void UGuestOptionWidget::OnQuitClicked()
{
	if (UGuestUISubsystem* UISys = GetUISubsystem())
	{
		UISys->PopWidget(GuestGameplayTags::TAG_WidgetStack_Modal);
	}
}

void UGuestOptionWidget::OnTabSelected(FName TabId)
{
	UE_LOG(LogTemp, Log, TEXT("[GuestOptionWidget] %s 탭으로 전환됨"), *TabId.ToString());
}