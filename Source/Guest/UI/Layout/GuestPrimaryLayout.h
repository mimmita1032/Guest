// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GuestPrimaryLayout.generated.h"

class UCommonActivatableWidgetContainerBase;

/**
 * UGuestPrimaryLayout
 *
 * 모든 Widget Stack 을 포함하는 최상위 레이아웃 위젯.
 * GuestPlayerController::CreatePrimaryLayout() 에서 생성하여 뷰포트에 추가.
 * NativeOnInitialized 에서 각 스택을 GuestUISubsystem 에 등록하여
 * 이후 PushWidgetByTag 가 올바른 스택을 찾을 수 있도록 함.
 *
 * [WBP 제작 가이드]
 * Content/UI/ 에 Widget Blueprint 생성.
 * Parent Class: GuestPrimaryLayout.
 * 아래 이름으로 CommonActivatableWidgetStack 4개 배치:
 *
 *   Stack_Frontend    (ZOrder 0) - 메인메뉴, PressAnyKey
 *   Stack_GameHUD     (ZOrder 1) - 상시 HUD
 *   Stack_GameMenu    (ZOrder 2) - 카메라 UI, 인벤토리 등
 *   Stack_BarDialogue (ZOrder 2) - 바 모드 전체화면 대화 (VA-11 HALL-A 스타일)
 *   Stack_Narration   (ZOrder 2) - 퀘스트 완료 등 스토리 분기 전환 나레이션 (전체화면)
 *   Stack_Modal       (ZOrder 3) - 확인창, 알림 (최상단)
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class GUEST_API UGuestPrimaryLayout : public UCommonUserWidget
{
    GENERATED_BODY()

protected:

    //~ Begin UUserWidget Interface
    /** 각 스택을 GuestUISubsystem 에 등록. BeginPlay 이전에 호출됨. */
    virtual void NativeOnInitialized() override;
    //~ End UUserWidget Interface

private:

    // ── Widget Stack 슬롯 ──
    // BindWidget: WBP 에서 동일한 이름의 위젯과 자동 연결됨.
    // 이름이 다르면 에디터에서 에러 발생.

    /** 타이틀 / 메인메뉴 레이어. ZOrder 0. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> Stack_Frontend;

    /** 인게임 HUD 레이어. ZOrder 1. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> Stack_GameHUD;

    /** 인게임 메뉴 레이어. ZOrder 2. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> Stack_GameMenu;

    /** 모달 레이어. ZOrder 3 (최상단). */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> Stack_Modal;

    /** 바 모드 전체화면 대화 레이어. ZOrder 2 (GameMenu와 동급, 별도 스택). */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> Stack_BarDialogue;

    /** 스토리 분기 전환 나레이션 레이어. ZOrder 2 (GameMenu와 동급, 별도 스택). */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> Stack_Narration;
};
