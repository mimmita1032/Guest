// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h"
#include "GuestMainMenuWidget.generated.h"

class UGuestCommonButton;
class UTextBlock;

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

    // ── 메뉴 버튼 (커스텀 사운드 버튼 적용) ──

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGuestCommonButton> Button_Continue;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGuestCommonButton> Button_NewGame;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGuestCommonButton> Button_Settings;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGuestCommonButton> Button_Quit;

    // ── 버튼 핸들러 ──

    UFUNCTION()
    void OnContinueClicked();

    UFUNCTION()
    void OnNewGameClicked();

    UFUNCTION()
    void OnSettingsClicked();

    UFUNCTION()
    void OnQuitClicked();

    // ── 내부 헬퍼 ──

    /** 세이브 데이터 존재 여부 확인. TODO: 실제 세이브 시스템 연동 필요. */
    bool HasSaveData() const;
};