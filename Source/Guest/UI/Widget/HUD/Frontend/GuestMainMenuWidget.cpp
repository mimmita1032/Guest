// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/HUD/Frontend/GuestMainMenuWidget.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"
#include "Guest/UI/Base/GuestCommonButton.h" 
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"

// ─────────────────────────────────────────────────────────
// UUserWidget 인터페이스
// ─────────────────────────────────────────────────────────

void UGuestMainMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // 1. [버튼 바인딩] 각 버튼에 클릭 델리게이트 연결.
    if (Button_StartGame) Button_StartGame->OnClicked().AddUObject(this, &UGuestMainMenuWidget::OnStartGameClicked);
    if (Button_Settings)  Button_Settings->OnClicked().AddUObject(this, &UGuestMainMenuWidget::OnSettingsClicked);
    if (Button_Quit)      Button_Quit->OnClicked().AddUObject(this,     &UGuestMainMenuWidget::OnQuitClicked);
}

// ─────────────────────────────────────────────────────────
// UCommonActivatableWidget 인터페이스
// ─────────────────────────────────────────────────────────

void UGuestMainMenuWidget::NativeOnActivated()
{
    Super::NativeOnActivated();
    
    // 메인 메뉴 진입 시 세이브 리스트는 우선 숨김 처리
    if (SaveProfileListWidget)
    {
        SaveProfileListWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

// ─────────────────────────────────────────────────────────
// 버튼 핸들러
// ─────────────────────────────────────────────────────────

void UGuestMainMenuWidget::OnStartGameClicked()
{
    UE_LOG(LogTemp, Log, TEXT("[MainMenu] 게임 시작 클릭 -> 세이브 프로필 리스트 열기"));

    // UI 서브시스템을 통해 새로 만든 MainMenuBoardWidget을 화면에 Push 합니다.
    if (UGuestUISubsystem* UISys = GetUISubsystem())
    {
        // TAG_Widget_SaveLoadBoard 부분은 프로젝트의 실제 태그나 클래스 구조에 맞게 설정해주세요.
        UISys->PushWidget(
            GuestGameplayTags::TAG_WidgetStack_Frontend,
            GuestGameplayTags::TAG_Widget_MainMenuSaveBoard 
        );
    }
}

void UGuestMainMenuWidget::OnSettingsClicked()
{
    // 설정 화면을 Modal 스택에 Push.
    if (UGuestUISubsystem* UISys = GetUISubsystem())
    {
        UISys->PushWidget(
            GuestGameplayTags::TAG_WidgetStack_Modal,
            GuestGameplayTags::TAG_Widget_Options
        );
    }
}

void UGuestMainMenuWidget::OnQuitClicked()
{
    // 게임 종료.
    UKismetSystemLibrary::QuitGame(
        this,
        GetOwningPlayer(),
        EQuitPreference::Quit,
        false
    );
}