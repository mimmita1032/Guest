// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "GuestPlayerController.generated.h"

/**
 * AGuestPlayerController
 * * 게임의 진입점 역할을 수행하며, UI 서브시스템 연동 및 입력 모드 제어를 담당합니다.
 * BeginPlay에서 PrimaryLayout을 생성하고, 디버그용 UI 및 Enhanced Input을 관리합니다.
 */
UCLASS()
class GUEST_API AGuestPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AGuestPlayerController();

protected:
    //~ Begin APlayerController Interface
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    //~ End APlayerController Interface

public:
    /** 인게임 모드: 마우스 커서 숨김, 게임 입력 전용. */
    UFUNCTION(BlueprintCallable, Category = "Guest|Input")
    void SetInputModeGameOnly();

    /** UI 모드: 마우스 커서 표시, UI 입력 전용. */
    UFUNCTION(BlueprintCallable, Category = "Guest|Input")
    void SetInputModeUIOnly();

    /** 혼합 모드: 게임 + UI 동시 입력. 인벤토리, 카메라 UI 등에 사용. */
    UFUNCTION(BlueprintCallable, Category = "Guest|Input")
    void SetInputModeGameAndUI();

    /** GuestUISubsystem에 빠르게 접근하기 위한 헬퍼 함수. */
    UFUNCTION(BlueprintPure, Category = "Guest|UI")
    class UGuestUISubsystem* GetUISubsystem() const;

private:
    /** PrimaryLayout 위젯을 뷰포트에 생성하고 Subsystem에 스택 등록. */
    void CreatePrimaryLayout();

protected:
    /** Enhanced Input: 기본 매핑 컨텍스트 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guest|Input")
    TObjectPtr<class UInputMappingContext> DefaultMappingContext;

#pragma region CommonUI
protected:
    /** 프로젝트 설정 혹은 디테일 패널에서 지정할 메인 레이아웃 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guest|UI")
    TSubclassOf<class UGuestPrimaryLayout> PrimaryLayoutClass;

    /** 생성된 메인 레이아웃 인스턴스 보관 */
    UPROPERTY(Transient)
    TObjectPtr<class UGuestPrimaryLayout> PrimaryLayoutInstance;
#pragma endregion

#pragma region DebugUI
protected:
    /** 디버그용 위젯 클래스 (예: WBP_DebugTime) */
    UPROPERTY(EditAnywhere, Category = "Guest|Debug")
    TSubclassOf<class UUserWidget> DebugWidgetClass;

    /** 디버그 UI 토글을 위한 입력 액션 (F1 등) */
    UPROPERTY(EditAnywhere, Category = "Guest|Input")
    TObjectPtr<class UInputAction> IA_ToggleDebugUI;

    /** 화면에 띄운 디버그 위젯 인스턴스 보관 */
    UPROPERTY(Transient)
    TObjectPtr<class UUserWidget> DebugWidgetInstance;

    /** 입력 액션에 바인딩될 토글 함수 */
    void ToggleDebugUI();
#pragma endregion
};