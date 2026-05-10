// Copyright (c) 2026 Anything Left Behind?. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Guest/UI/Types/GuestUITypes.h"
#include "GuestUISubsystem.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;

/**  스타일: 위젯이 푸시되었을 때 알림을 주는 델리게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetPushed, UCommonActivatableWidget*, Widget);

UCLASS()
class GUEST_API UGuestUISubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    //~ Begin USubsystem Interface
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Deinitialize() override;
    //~ End USubsystem Interface

    /** 방식: 컨테이너(Stack) 등록 */
    void RegisterStack(FGameplayTag StackTag, UCommonActivatableWidgetContainerBase* Stack);

    /** *설계의 핵심 함수
     * @param StackTag  어느 레이어에 띄울 것인가 (예: Guest.Stack.GameMenu)
     * @param WidgetTag 무엇을 띄울 것인가 (예: Guest.Widget.Inventory)
     */
    UFUNCTION(BlueprintCallable, Category = "UI", meta = (DisplayName = "Push Widget Stack"))
    void PushWidget(FGameplayTag StackTag, FGameplayTag WidgetTag);

    /** 스택의 최상위 위젯 제거 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void PopWidget(FGameplayTag StackTag);

    /** NPC 대화 위젯을 GameMenu 스택에 열고 데이터를 전달한다. */
    UFUNCTION(BlueprintCallable, Category = "UI|Dialogue")
    void OpenNPCDialogue(const FNPCDialogueData& Data);

    /** 위젯이 활성화된 직후 대화 데이터를 가져갈 때 사용. */
    const FNPCDialogueData& GetPendingDialogueData() const { return PendingDialogueData; }

    /** 전역 알림용 델리게이트 */
    UPROPERTY(BlueprintAssignable, Category = "UI")
    FOnWidgetPushed OnWidgetPushed;

private:
    UPROPERTY()
    TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> StackMap;

    FNPCDialogueData PendingDialogueData;
};