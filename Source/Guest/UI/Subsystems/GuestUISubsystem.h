// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Guest/UI/Types/GuestUITypes.h"
#include "GuestUISubsystem.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;

/* 위젯 Push 완료 시 호출되는 델리게이트. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetPushed, UCommonActivatableWidget*, PushedWidget);

/* 확인 모달 결과 콜백. */
DECLARE_DELEGATE_OneParam(FOnConfirmResult, EGuestConfirmResult);


/**
 * UGuestUISubsystem
 *
 * Guest 게임 중앙 UI 관리자.
 * GameInstance Subsystem 으로 레벨 전환 간 파괴되지 않음.
 *
 * 사용법:
 *   UGuestUISubsystem* UISys = GetGameInstance()->GetSubsystem<UGuestUISubsystem>();
 *   UISys->PushWidgetByTag(GuestGameplayTags::TAG_Widget_CameraUI);
 */
UCLASS()
class GUESTUI_API UGuestUISubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    //~ Begin USubsystem Interface
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    //~ End USubsystem Interface

    // ── Primary Layout 관리 ──

    /* GuestPrimaryLayout 이 NativeOnInitialized 에서 스택을 등록할 때 호출. */
    void RegisterStack(const FGameplayTag& StackTag, UCommonActivatableWidgetContainerBase* Stack);

    /* StackTag 에 해당하는 컨테이너 반환. */
    UCommonActivatableWidgetContainerBase* GetWidgetStack(const FGameplayTag& StackTag) const;

    // ── 위젯 Push / Pop ──

    /**
     * Tag 에 매핑된 위젯을 비동기 로드 후 적절한 스택에 Push.
     * @param WidgetTag     Guest.Widget.* 태그
     * @param OnPushed      Push 완료 콜백 (선택)
     */
    UFUNCTION(BlueprintCallable, Category = "Guest|UI")
    void PushWidgetByTag(const FGameplayTag& WidgetTag,
                         FOnWidgetPushed OnPushed = FOnWidgetPushed());

    /* 지정 스택 최상단 위젯 Pop. */
    UFUNCTION(BlueprintCallable, Category = "Guest|UI")
    void PopTopWidget(const FGameplayTag& StackTag);

    // ── 확인 모달 ──

    /* 확인/취소 모달 표시. */
    void ShowConfirmModal(const FGuestConfirmData& ConfirmData, FOnConfirmResult OnResult);

    // ── NPC 대화 ──

    /* NPC 대화창 열기. 대화 데이터를 임시 저장 후 위젯이 읽어감. */
    UFUNCTION(BlueprintCallable, Category = "Guest|UI")
    void OpenNPCDialogue(const FText& NPCName, const TArray<FText>& DialogueLines);

    /* 대화창 위젯이 읽어갈 현재 대화 데이터. */
    FText         CurrentNPCName;
    TArray<FText> CurrentDialogueLines;

    /* 위젯 Push 완료 이벤트. */
    UPROPERTY(BlueprintAssignable, Category = "Guest|UI")
    FOnWidgetPushed OnWidgetPushed;

private:

    /* Stack 태그 → 컨테이너 캐시. */
    TMap<FGameplayTag, TWeakObjectPtr<UCommonActivatableWidgetContainerBase>> StackMap;

    /* 위젯 태그 → 레이어 스택 태그 결정. */
    FGameplayTag ResolveStackTag(const FGameplayTag& WidgetTag) const;
};
