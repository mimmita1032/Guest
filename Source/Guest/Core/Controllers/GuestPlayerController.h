// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GuestPlayerController.generated.h"

class UGuestUISubsystem;
class UGuestPrimaryLayout;

/**
 * AGuestPlayerController
 * * UI 프레임워크와 연동되어 최상위 레이아웃(Primary Layout)을 생성하고 관리합니다.
 */
UCLASS()
class GUEST_API AGuestPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AGuestPlayerController();

    /** UI 서브시스템에 빠르게 접근하기 위한 헬퍼 */
    UFUNCTION(BlueprintPure, Category = "Guest|UI")
    UGuestUISubsystem* GetUISubsystem() const;

    /** 현재 활성화된 최상위 레이아웃 반환 */
    UFUNCTION(BlueprintPure, Category = "Guest|UI")
    UGuestPrimaryLayout* GetPrimaryLayout() const { return PrimaryLayout; }

protected:
    virtual void BeginPlay() override;

    /** * GuestUISettings에 정의된 PrimaryLayoutClass를 생성하여 뷰포트에 추가합니다.
     * 폰(Pawn)이 생성되기 전, UI 환경을 먼저 구축하기 위해 호출됩니다.
     */
    virtual void CreatePrimaryLayout();

private:
    /** 생성된 최상위 레이아웃 인스턴스 보관 */
    UPROPERTY()
    TObjectPtr<UGuestPrimaryLayout> PrimaryLayout;
};