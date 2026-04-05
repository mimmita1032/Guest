// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GuestActivatableBase.generated.h"

class AGuestPlayerController;
class UGuestUISubsystem;

/**
 * UGuestActivatableBase
 *
 * Guest 게임의 모든 활성화 가능 UI 위젯이 상속받는 베이스 클래스.
 * CommonUI 스택에 Push / Pop 되는 모든 화면은 이 클래스를 상속받아 제작.
 *
 * 제공 기능:
 *  - GuestPlayerController 캐시된 접근 획득.
 *  - GuestUISubsystem 빠른 접근 획득.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class GUESTUI_API UGuestActivatableBase : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:

    //~ Begin UCommonActivatableWidget Interface
    /* 위젯 활성화 시 로그 출력 수행. */
    virtual void NativeOnActivated() override;
    /* 위젯 비활성화 시 로그 출력 수행. */
    virtual void NativeOnDeactivated() override;
    //~ End UCommonActivatableWidget Interface

    /* 소유 GuestPlayerController 반환. 반복 Cast 방지를 위해 캐싱 수행. */
    UFUNCTION(BlueprintPure, Category = "Guest|UI")
    AGuestPlayerController* GetGuestPlayerController() const;

    /* GuestUISubsystem 반환. */
    UFUNCTION(BlueprintPure, Category = "Guest|UI")
    UGuestUISubsystem* GetUISubsystem() const;

private:

    /* 반복 Cast 방지용 PlayerController 캐시. */
    mutable TWeakObjectPtr<AGuestPlayerController> CachedController;
};
