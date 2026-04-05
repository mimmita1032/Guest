// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Core/Controllers/GuestPlayerController.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/UI/Settings/GuestUISettings.h"
#include "Guest/UI/GameplayTags/GuestGameplayTags.h"
#include "Guest/UI/Layout/GuestPrimaryLayout.h"
#include "Guest/Utils/GLog.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"

AGuestPlayerController::AGuestPlayerController()
{
    bShowMouseCursor = false;
}

void AGuestPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 1. Enhanced Input Subsystem 설정
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // 2. 초기 입력 모드 설정
    SetInputModeGameOnly();

    // 3. 로컬 컨트롤러일 경우 Primary Layout 생성 및 초기 위젯 Push
    if (IsLocalController())
    {
        CreatePrimaryLayout();
    }
}

void AGuestPlayerController::CreatePrimaryLayout()
{
    const UGuestUISettings* Settings = GetDefault<UGuestUISettings>();
    if (!Settings) return;

    // [PrimaryLayout 로드] 동기 로드.
    UClass* LayoutClass = Settings->PrimaryLayoutClass.LoadSynchronous();
    
    if (LayoutClass)
    {
        // 위젯 생성 및 인스턴스 저장 (UGuestPrimaryLayout 타입 캐스팅)
        PrimaryLayoutInstance = CreateWidget<UGuestPrimaryLayout>(this, LayoutClass);
        if (PrimaryLayoutInstance)
        {
            PrimaryLayoutInstance->AddToViewport(0);
            G_LOG(TEXT("Primary Layout 생성 및 뷰포트 추가 완료"));

            // [첫 화면 표시] 서브시스템을 통해 PressAnyKey 화면으로 시작
            if (UGuestUISubsystem* UISys = GetUISubsystem())
            {
                UISys->PushWidgetByTag(GuestGameplayTags::TAG_Widget_PressAnyKey);
            }
        }
    }
    else
    {
        G_ERR(TEXT("PrimaryLayoutClass가 비어있습니다! Project Settings > Guest UI Settings를 확인하세요."));
    }
}

// ─────────────────────────────────────────────────────────
// 입력 모드 관리 함수
// ─────────────────────────────────────────────────────────

void AGuestPlayerController::SetInputModeGameOnly()
{
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = false;
}

void AGuestPlayerController::SetInputModeUIOnly()
{
    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}

void AGuestPlayerController::SetInputModeGameAndUI()
{
    FInputModeGameAndUI Mode;
    Mode.SetHideCursorDuringCapture(false);
    SetInputMode(Mode);
    bShowMouseCursor = true;
}

// ─────────────────────────────────────────────────────────
// 접근자
// ─────────────────────────────────────────────────────────

UGuestUISubsystem* AGuestPlayerController::GetUISubsystem() const
{
    if (UGameInstance* GI = GetGameInstance())
    {
        return GI->GetSubsystem<UGuestUISubsystem>();
    }
    return nullptr;
}

#pragma region DebugUI
void AGuestPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (IA_ToggleDebugUI)
        {
            EIC->BindAction(IA_ToggleDebugUI, ETriggerEvent::Started, this, &AGuestPlayerController::ToggleDebugUI);
        }
    }
}

void AGuestPlayerController::ToggleDebugUI()
{
    if (!DebugWidgetClass)
    {
        G_ERR(TEXT("DebugWidgetClass가 할당되지 않았습니다! BP_GuestPlayerController의 디테일 패널을 확인하세요."));
        return;
    }

    // 위젯이 아직 생성되지 않았다면 최초 1회 생성하여 메모리에 할당
    if (!DebugWidgetInstance && DebugWidgetClass)
    {
        DebugWidgetInstance = CreateWidget<UUserWidget>(this, DebugWidgetClass);
    }

    if (DebugWidgetInstance)
    {
        if (DebugWidgetInstance->IsInViewport())
        {
            // [끄기] 화면에서 제거하고 조작을 게임 전용으로 변경
            DebugWidgetInstance->RemoveFromParent();
            SetInputModeGameOnly();
            
            G_LOG(TEXT("디버그 UI 비활성화"));
        }
        else
        {
            // [켜기] 화면에 추가하고 조작을 UI 포함 모드로 변경
            DebugWidgetInstance->AddToViewport();
            
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(DebugWidgetInstance->TakeWidget());
            SetInputMode(InputMode);
            bShowMouseCursor = true;
            
            G_LOG(TEXT("디버그 UI 활성화"));
        }
    }
}
#pragma endregion