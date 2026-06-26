// Copyright (c) 2026 Anything Left Behind?. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "GuestUIInputConfig.h"
#include "Guest/UI/Subsystems/GuestUIInputConfig.h"
#include "Guest/UI/Types/GuestUITypes.h"
#include "GuestUISubsystem.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;
class UInputMappingContext;
class APlayerController;

/**  스타일: 위젯이 푸시되었을 때 알림을 주는 델리게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetPushed, UCommonActivatableWidget*, Widget);

/**
 * UGuestUISubsystem
 * Guest 게임의 중앙 UI + 입력 통합 관리자.
 */
UCLASS(BlueprintType)
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
    
    /** NPC 대화 위젯을 GameMenu 스택에 열고 데이터를 전달한다. (Skyrim 스타일) */
    UFUNCTION(BlueprintCallable, Category = "UI|Dialogue")
    void OpenNPCDialogue(const FNPCDialogueData& Data);

    /** 위젯이 활성화된 직후 대화 데이터를 가져갈 때 사용. */
    const FNPCDialogueData& GetPendingDialogueData() const { return PendingDialogueData; }

    /** 바 모드 전체화면 대화 위젯을 BarDialogue 스택에 열고 데이터를 전달한다. (VA-11 HALL-A 스타일) */
    UFUNCTION(BlueprintCallable, Category = "UI|Dialogue")
    void OpenBarDialogue(const FBarDialogueData& Data, AActor* DialogueActor = nullptr);

    /** 바 모드 위젯이 활성화된 직후 데이터를 가져갈 때 사용. */
    const FBarDialogueData& GetPendingBarDialogueData() const { return PendingBarDialogueData; }

    /** 현재 대화 중인 바 NPC 액터 반환. 카메라 블렌드에 사용. */
    AActor* GetPendingBarDialogueActor() const { return PendingBarDialogueActor.Get(); }

    /** 전역 알림용 델리게이트 */
    UPROPERTY(BlueprintAssignable, Category = "UI")
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

    FNPCDialogueData PendingDialogueData;
    FBarDialogueData PendingBarDialogueData;

    UPROPERTY()
    TWeakObjectPtr<AActor> PendingBarDialogueActor;
};