// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Widget/GuestActivatableBase.h"
#include "GuestMainMenuWidget.generated.h"

class UCommonButtonBase;
class UTextBlock;

/**
 * UGuestMainMenuWidget
 *
 * 메인 메뉴 화면.
 * GuestActivatableBase 를 상속하여 CommonUI 스택(Frontend) 에서 관리됨.
 *
 * [사운드 처리 방식]
 * - 화면 Open / Close : GuestActivatableBase 에서 자동 처리
 *                       WBP 에디터에서 AudioData 슬롯에 DA_GuestSound 지정만 하면 됨.
 * - 버튼 클릭 사운드  : 각 핸들러에서 직접 호출
 *                       버튼마다 다른 사운드를 줄 수 있음.
 *
 * [WBP 제작 가이드]
 * Parent Class: GuestMainMenuWidget
 * BindWidget 이름 목록:
 *   Text_Title        - 타이틀 텍스트
 *   Button_Continue   - 계속 버튼
 *   Button_NewGame    - 새로운 게임 버튼
 *   Button_Settings   - 설정 버튼
 *   Button_Quit       - 종료 버튼
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class GUEST_API UGuestMainMenuWidget : public UGuestActivatableBase
{
    GENERATED_BODY()

protected:

    //~ Begin UUserWidget Interface
    /** 버튼 델리게이트 바인딩 수행. */
    virtual void NativeOnInitialized() override;
    //~ End UUserWidget Interface

    //~ Begin UCommonActivatableWidget Interface
    /**
     * 활성화 시 세이브 데이터 유무 확인하여
     * 계속하기 버튼 활성/비활성 처리.
     * Open 사운드는 Super::NativeOnActivated() 에서 자동 처리됨.
     */
    virtual void NativeOnActivated() override;
    //~ End UCommonActivatableWidget Interface

private:

    // ── 타이틀 텍스트 ──

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Text_Title;

    // ── 메뉴 버튼 ──

    /** 계속하기. 세이브 데이터가 없으면 비활성화. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonButtonBase> Button_Continue;

    /** 새로운 게임 시작. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonButtonBase> Button_NewGame;

    /** 설정 화면으로 이동. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonButtonBase> Button_Settings;

    /** 게임 종료. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonButtonBase> Button_Quit;

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

    /** 버튼 클릭 사운드 재생. AudioData 없으면 스킵. */
    void PlayButtonClickSound();

    /** 세이브 데이터 존재 여부 확인. TODO: 실제 세이브 시스템 연동 필요. */
    bool HasSaveData() const;
};