// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/HUD/Frontend/GuestMainMenuWidget.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"
#include "Guest/UI/Base/GuestCommonButton.h" 
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Guest/Save/GuestSaveGame.h"
#include "Guest/Save/GuestSaveSlotNames.h"
#include "Guest/Core/GameInstance/GuestGameInstance.h"
#include "Guest/Utils/GLog.h"

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
    const FString SlotName = FindLatestSaveSlotName();
    if (SlotName.IsEmpty())
    {
        G_WARN(TEXT("[MainMenu] 불러올 세이브가 없습니다."));
        return;
    }

    UGuestGameInstance* GI = Cast<UGuestGameInstance>(GetGameInstance());
    if (!GI) return;

    G_LOG(TEXT("[MainMenu] 이어하기: %s"), *SlotName);

    // RequestLoadFromSlot이 저장된 맵으로의 전환까지 맡는다.
    GI->RequestLoadFromSlot(SlotName, GuestSaveSlots::DefaultUserIndex());
}

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

// ─────────────────────────────────────────────────────────
// 내부 헬퍼
// ─────────────────────────────────────────────────────────

FString UGuestMainMenuWidget::FindLatestSaveSlotName() const
{
    const int32 UserIndex = GuestSaveSlots::DefaultUserIndex();

    FString BestSlot;
    FDateTime BestTime = FDateTime::MinValue();

    // 슬롯 개수는 LoadBoard와 같은 값을 쓴다. 한쪽만 늘리면 최근 세이브를 놓친다.
    for (int32 Index = 0; Index < NumSaveSlots; ++Index)
    {
        const FString SlotName = GuestSaveSlots::MakeSlotName(Index);
        if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex)) continue;

        const UGuestSaveGame* Save = Cast<UGuestSaveGame>(
            UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
        if (!Save) continue;

        if (Save->SavedAt > BestTime)
        {
            BestTime = Save->SavedAt;
            BestSlot = SlotName;
        }
    }

    return BestSlot;
}

bool UGuestMainMenuWidget::HasSaveData() const
{
    return !FindLatestSaveSlotName().IsEmpty();
}