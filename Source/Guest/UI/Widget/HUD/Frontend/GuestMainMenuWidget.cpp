// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/HUD/Frontend/GuestMainMenuWidget.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/UI/GameplayTags/GuestGameplayTags.h"
#include "Guest/UI/buttonbase/GuestCommonButton.h" // 커스텀 사운드 버튼 클래스 포함
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"

// ─────────────────────────────────────────────────────────
// UUserWidget 인터페이스
// ─────────────────────────────────────────────────────────

void UGuestMainMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // 1. [버튼 바인딩] 각 버튼에 클릭 델리게이트 연결.
    if (Button_Continue) Button_Continue->OnClicked().AddUObject(this, &UGuestMainMenuWidget::OnContinueClicked);
    if (Button_NewGame)  Button_NewGame->OnClicked().AddUObject(this,  &UGuestMainMenuWidget::OnNewGameClicked);
    if (Button_Settings) Button_Settings->OnClicked().AddUObject(this, &UGuestMainMenuWidget::OnSettingsClicked);
    if (Button_Quit)     Button_Quit->OnClicked().AddUObject(this,     &UGuestMainMenuWidget::OnQuitClicked);
}

// ─────────────────────────────────────────────────────────
// UCommonActivatableWidget 인터페이스
// ─────────────────────────────────────────────────────────

void UGuestMainMenuWidget::NativeOnActivated()
{
    // Open 사운드는 Super 에서 자동 처리됨.
    Super::NativeOnActivated();

    // 1. [계속하기 버튼 상태 처리]
    // 세이브 데이터가 없으면 계속하기 버튼 비활성화.
    if (Button_Continue)
    {
        Button_Continue->SetIsEnabled(HasSaveData());
    }
}

// ─────────────────────────────────────────────────────────
// 버튼 핸들러 (사운드 재생 로직은 커스텀 버튼으로 위임됨)
// ─────────────────────────────────────────────────────────

void UGuestMainMenuWidget::OnContinueClicked()
{
    // TODO: 세이브 데이터 로드 후 인게임 레벨로 전환.
    UE_LOG(LogTemp, Log, TEXT("[MainMenu] 계속하기 클릭."));
}

void UGuestMainMenuWidget::OnNewGameClicked()
{
    // TODO: 새 게임 시작 로직 (세이브 초기화 후 인게임 레벨 로드).
    UE_LOG(LogTemp, Log, TEXT("[MainMenu] 새로운 게임 클릭."));
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

// ─────────────────────────────────────────────────────────
// 내부 헬퍼
// ─────────────────────────────────────────────────────────

bool UGuestMainMenuWidget::HasSaveData() const
{
    // TODO: 실제 세이브 시스템 연동 후 구현.
    return false;
}