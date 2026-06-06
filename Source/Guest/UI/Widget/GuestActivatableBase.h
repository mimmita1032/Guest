// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GuestActivatableBase.generated.h"

class AGuestPlayerController;
class UGuestUISubsystem;
class UGuestAudioDataAsset;
class UGuestSoundSubsystem;

/**
 * UGuestActivatableBase
 *
 * Guest 게임의 모든 활성화 가능 UI 위젯이 상속받는 베이스 클래스.
 * CommonUI 스택에 Push / Pop 되는 모든 화면은 이 클래스를 상속받아 제작.
 *
 * 제공 기능:
 *  - GuestPlayerController 캐시된 접근 (반복 Cast 방지)
 *  - GuestUISubsystem / GuestSoundSubsystem 빠른 접근
 *  - 화면 Open / Close 시 사운드 자동 재생
 *    → AudioData 에셋을 WBP 에디터에서 지정하면 별도 코드 없이 동작
 *    → 사운드가 필요 없는 위젯은 AudioData 를 비워두면 자동으로 스킵됨
 *
 * [상속 구조]
 * UCommonActivatableWidget
 *   └── UGuestActivatableBase         ← 이 클래스
 *         ├── UGuestMainMenuWidget
 *         ├── UGuestHUD
 *         └── UGuestCameraUI  ... 등 모든 게임 화면
 *
 * [주의] WBP 에서 이 클래스를 직접 Parent 로 지정하지 말 것.
 * 반드시 목적에 맞는 하위 C++ 클래스를 거쳐야 함.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class GUEST_API UGuestActivatableBase : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:

    //~ Begin UCommonActivatableWidget Interface
    /**
     * 위젯 활성화 시 Open 사운드 자동 재생.
     * AudioData 가 비어있으면 사운드 스킵.
     * 하위 클래스에서 Override 시 Super:: 반드시 호출.
     */
    virtual void NativeOnActivated() override;

    /**
     * 위젯 비활성화 시 Close 사운드 자동 재생.
     * AudioData 가 비어있으면 사운드 스킵.
     * 하위 클래스에서 Override 시 Super:: 반드시 호출.
     */
    virtual void NativeOnDeactivated() override;
    //~ End UCommonActivatableWidget Interface

    /**
     * 소유 GuestPlayerController 반환.
     * 내부적으로 캐싱하여 매 호출마다 Cast 가 발생하지 않도록 최적화.
     */
    UFUNCTION(BlueprintPure, Category = "Guest|UI")
    AGuestPlayerController* GetGuestPlayerController() const;

    /** GuestUISubsystem 반환. */
    UFUNCTION(BlueprintPure, Category = "Guest|UI")
    UGuestUISubsystem* GetUISubsystem() const;

    /** GuestSoundSubsystem 반환. */
    UFUNCTION(BlueprintPure, Category = "Guest|UI")
    UGuestSoundSubsystem* GetSoundSubsystem() const;

    /**
     * 이 위젯의 Open / Close 사운드 데이터 에셋.
     * WBP 에디터의 Details 패널에서 지정.
     * 비워두면 이 위젯은 Open / Close 사운드 없이 동작.
     *
     * DA_GuestSound 의 AudioMap 에 아래 태그가 등록되어 있어야 함:
     *  - Guest.Sound.Event.UI.ScreenOpen
     *  - Guest.Sound.Event.UI.ScreenClose
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guest|Sound")
    TObjectPtr<UGuestAudioDataAsset> AudioData;

private:

    /**
     * 반복 Cast 방지용 PlayerController 캐시.
     * TWeakObjectPtr 사용으로 PlayerController 가 파괴되어도 크래시 방지.
     */
    mutable TWeakObjectPtr<AGuestPlayerController> CachedController;
};