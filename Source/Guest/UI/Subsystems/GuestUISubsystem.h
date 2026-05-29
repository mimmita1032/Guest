// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "GuestUIInputConfig.h"
#include "Guest/UI/Subsystems/GuestUIInputConfig.h"
#include "GuestUISubsystem.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;
class UInputMappingContext;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetPushed, UCommonActivatableWidget*, Widget);

/**
 * UGuestUISubsystem
 * Guest 게임의 중앙 UI + 입력 통합 관리자.
 */
UCLASS()
class GUEST_API UGuestUISubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    //~ Begin USubsystem Interface
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Deinitialize() override;
    //~ End USubsystem Interface

    // ── Stack 관리 ──
    void RegisterStack(FGameplayTag StackTag, UCommonActivatableWidgetContainerBase* Stack);

    // ── 입력 Config 등록 ──
    void RegisterInputConfig(const FGameplayTag& StackTag, const FGuestUIInputConfig& Config);

    // ── 위젯 Push / Pop ──
    UFUNCTION(BlueprintCallable, Category = "Guest|UI", meta = (DisplayName = "Push Widget"))
    void PushWidget(FGameplayTag StackTag, FGameplayTag WidgetTag);

    UFUNCTION(BlueprintCallable, Category = "Guest|UI")
    void PopWidget(FGameplayTag StackTag);

    UPROPERTY(BlueprintAssignable, Category = "Guest|UI")
    FOnWidgetPushed OnWidgetPushed;

    UFUNCTION(BlueprintPure, Category = "Guest|UI")
    EGuestInputMode GetCurrentInputMode() const { return CurrentInputMode; }

    UFUNCTION(BlueprintPure, Category = "Guest|UI")
    FGameplayTag GetCurrentStackTag() const { return CurrentStackTag; }

private:

    // ── Stack 데이터 ──
    UPROPERTY()
    TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> StackMap;

    /** 활성화된 스택의 히스토리. */
    UPROPERTY()
    TArray<FGameplayTag> ActiveStackHistory;

    // ── 입력 데이터 ──
    TMap<FGameplayTag, FGuestUIInputConfig> InputConfigMap;
    FGameplayTag CurrentStackTag;
    EGuestInputMode CurrentInputMode = EGuestInputMode::GameOnly;

    UPROPERTY()
    TObjectPtr<UInputMappingContext> CurrentIMC = nullptr;

    // ── 입력 내부 헬퍼 ──
    FGuestUIInputConfig ResolveInputConfig(const FGameplayTag& StackTag) const;
    void ApplyInputConfig(const FGuestUIInputConfig& Config);
    void ApplyInputMode(EGuestInputMode InputMode, APlayerController* PC);
    void SwapIMC(UInputMappingContext* NewIMC, int32 Priority, APlayerController* PC);
    APlayerController* GetLocalPlayerController() const;
};