// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h"
#include "GuestNarrationWidgetBase.generated.h"

class UCommonTextBlock;
class UCommonButtonBase;
class UImage;
class UWidgetAnimation;
class UGNarrationDataAsset;

// 진행 중인 UMG 애니메이션 종료 후 다음에 무엇을 할지 구분 (같은 완료 델리게이트를 여러 애니메이션이 공유하기 위함)
UENUM()
enum class ENarrationTransition : uint8
{
	None,
	ImageFadeOut,
	ScreenFadeOut
};

/**
 * 정지 일러스트 나레이션 연출 공통 베이스.
 * 퀘스트 완료 시(GQuestSubsystem::OnNarrationRequested → GuestUISubsystem::OpenNarration)
 * Narration 스택에 Push되어 재생된다.
 *
 * 필수 BindWidget (WBP에서 구성):
 *  - Image_Illustration : 정지 일러스트
 *  - Text_Narration      : 나레이션 텍스트
 *  - Btn_Advance         : 클릭 시 다음 장면으로 즉시 진행 (자동 진행 타이머도 취소됨)
 *
 * 선택 BindWidgetAnim (없으면 즉시 컷 전환 — 애니메이션 없이도 기능은 동작):
 *  - Anim_ScreenFadeIn / Anim_ScreenFadeOut : 나레이션 진입/종료 시 화면 전체 페이드
 *  - Anim_ImageFadeOut / Anim_ImageFadeIn   : 장면 전환 시 일러스트 페이드
 */
UCLASS(Abstract)
class GUEST_API UGuestNarrationWidgetBase : public UGuestActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	/** 서브클래스 NativeOnActivated에서 호출. 세션 초기화 후 첫 장면 표시. */
	void StartNarrationSession(UGNarrationDataAsset* InAsset);

	/** 서브클래스 NativeOnDeactivated에서 호출. 세션 상태/타이머 초기화. */
	void ResetNarrationSession();

	UPROPERTY()
	TObjectPtr<UGNarrationDataAsset> CurrentNarrationAsset;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Illustration;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Text_Narration;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> Btn_Advance;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_ScreenFadeIn;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_ScreenFadeOut;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_ImageFadeOut;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_ImageFadeIn;

	int32 CurrentBeatIndex = INDEX_NONE;
	ENarrationTransition PendingTransition = ENarrationTransition::None;
	FTimerHandle AutoAdvanceTimerHandle;

	// 현재 CurrentBeatIndex의 일러스트/텍스트를 적용하고 AutoAdvanceDelay가 있으면 타이머 예약
	void ShowBeat(int32 BeatIndex);

	// 다음 장면으로 진행 (마지막 장면이었다면 나레이션 종료)
	void AdvanceBeat();

	void FinishNarration();

	UFUNCTION()
	void HandleAnimFinished();

	UFUNCTION()
	void OnAdvanceClicked();
};
