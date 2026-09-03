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

    /** 훑을 세이브 슬롯 개수. LoadBoard(NumSlots)와 같은 값을 유지할 것. */
    static constexpr int32 NumSaveSlots = 4;

    /**
     * 가장 최근에 저장된 슬롯 이름을 찾는다. 없으면 빈 문자열.
     *
     * 이어하기는 슬롯을 고르게 하지 않고 마지막에 저장한 것을 연다.
     * 슬롯을 고르는 화면은 LoadBoard가 따로 있다.
     */
    FString FindLatestSaveSlotName() const;

    /** 세이브 데이터 존재 여부 확인. */
    bool HasSaveData() const;
};