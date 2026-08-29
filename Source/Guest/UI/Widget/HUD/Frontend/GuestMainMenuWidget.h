// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h"
#include "GuestMainMenuWidget.generated.h"

class UGuestCommonButton;
class UTextBlock;
class UUserWidget; // 세이브 프로필 리스트 위젯을 위해 추가

/**
 * UGuestMainMenuWidget
 *
 * 메인 메뉴 화면.
 * GuestActivatableBase 를 상속하여 CommonUI 스택(Frontend) 에서 관리됨.
 *
 * [사운드 처리 방식]
 * - 화면 Open / Close : GuestActivatableBase 에서 자동 처리
 * - 버튼 클릭 사운드  : UGuestCommonButton 내부에서 자동 처리되므로 메뉴 위젯 단에서 호출 불필요
 */

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class GUEST_API UGuestMainMenuWidget : public UGuestActivatableBase
{
    GENERATED_BODY()

protected:

    //~ Begin UUserWidget Interface
    virtual void NativeOnInitialized() override;
    //~ End UUserWidget Interface

    //~ Begin UCommonActivatableWidget Interface
    virtual void NativeOnActivated() override;
    //~ End UCommonActivatableWidget Interface

private:

    // ── 타이틀 텍스트 ──

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Text_Title;

    // ── 메뉴 버튼 (통합됨) ──

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGuestCommonButton> Button_StartGame; // 기존 계속하기/새 게임 통합

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGuestCommonButton> Button_Settings;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGuestCommonButton> Button_Quit;

    // ── 서브 위젯 (세이브 프로필 리스트) ──
    
    // 에디터에서 WBP_MainMenu 안에 세이브 리스트 위젯을 배치해두고 숨겨놓은 상태로 연결합니다.
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UUserWidget> SaveProfileListWidget; 

    // ── 버튼 핸들러 ──

    UFUNCTION()
    void OnStartGameClicked();

    UFUNCTION()
    void OnSettingsClicked();

    UFUNCTION()
    void OnQuitClicked();
};