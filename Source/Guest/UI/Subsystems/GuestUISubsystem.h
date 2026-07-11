// Copyright (c) 2026 Anything Left Behind?. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "CommonUITypes.h"
#include "GuestUIInputConfig.h"
#include "Guest/UI/Subsystems/GuestUIInputConfig.h"
#include "Guest/UI/Types/GuestUITypes.h"
#include "GuestUISubsystem.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;
class UInputMappingContext;
class APlayerController;
class UGDialogueDataAsset;

/** 위젯이 푸시되었을 때 알림을 주는 델리게이트 */
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

	/** 위젯이 NativeOnDeactivated에서 직접 호출해 입력 모드를 복구한다. */
	void NotifyWidgetDeactivated(FGameplayTag StackTag);

	/** 지정한 스택의 현재 활성 위젯이 WidgetTag에 해당하는 클래스인지 조회한다. */
	UFUNCTION(BlueprintPure, Category = "Guest|UI")
	bool IsWidgetActive(FGameplayTag StackTag, FGameplayTag WidgetTag) const;

	/**
	 * 바 모드 전체화면 대화를 BarDialogue 스택에 열고 세션을 시작한다.
	 * 위젯 활성화 시 GetPendingDialogueAsset()으로 데이터를 가져간다.
	 */
	UFUNCTION(BlueprintCallable, Category = "UI|Dialogue")
	void OpenBarDialogue(UGDialogueDataAsset* DialogueAsset, AActor* NPCActor = nullptr);

	/**
	 * 스카이림 스타일 NPC 대화를 GameMenu 스택에 열고 세션을 시작한다.
	 * 위젯 활성화 시 GetPendingDialogueAsset()으로 데이터를 가져간다.
	 */
	UFUNCTION(BlueprintCallable, Category = "UI|Dialogue")
	void OpenNPCDialogue(UGDialogueDataAsset* DialogueAsset);

	/** 위젯 NativeOnActivated에서 대화 에셋을 가져올 때 사용. */
	UGDialogueDataAsset* GetPendingDialogueAsset() const { return PendingDialogueAsset.Get(); }

	/** 현재 대화 중인 NPC 액터 반환. 카메라 블렌드에 사용. */
	AActor* GetPendingDialogueNPCActor() const { return PendingDialogueNPCActor.Get(); }

	/** 전역 알림용 델리게이트 */
	UPROPERTY(BlueprintAssignable, Category = "UI")
	FOnWidgetPushed OnWidgetPushed;

	UFUNCTION(BlueprintPure, Category = "Guest|UI")
	EGuestInputMode GetCurrentInputMode() const { return CurrentInputMode; }

	/**
	 * 현재 GuestUISubsystem이 적용 중인 입력모드를 CommonUI의 FUIInputConfig로 변환해 반환.
	 * 각 UGuestActivatableBase 파생 위젯의 GetDesiredInputConfig()는 이 값을 그대로 반환해야 한다 —
	 * CommonUI ActionRouter와 GuestUISubsystem이 서로 다른 입력모드를 주장하는 상황을 방지하기 위함.
	 */
	UFUNCTION(BlueprintPure, Category = "Guest|UI")
	FUIInputConfig GetDesiredUIInputConfig() const;

	UFUNCTION(BlueprintPure, Category = "Guest|UI")
	FGameplayTag GetCurrentStackTag() const { return CurrentStackTag; }

private:

	// ── Stack 데이터 ──
	UPROPERTY()
	TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> StackMap;

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

	// ── 대화 세션 ──
	UPROPERTY()
	TObjectPtr<UGDialogueDataAsset> PendingDialogueAsset;

	UPROPERTY()
	TWeakObjectPtr<AActor> PendingDialogueNPCActor;
};
