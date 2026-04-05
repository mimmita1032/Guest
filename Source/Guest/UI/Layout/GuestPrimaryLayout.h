// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GuestPrimaryLayout.generated.h"

class UCommonActivatableWidgetContainerBase;

/**
 * UGuestPrimaryLayout
 *
 * 모든 Widget Stack 을 포함하는 최상위 레이아웃 위젯.
 * GuestPlayerController 가 뷰포트에 생성하며,
 * NativeOnInitialized 에서 각 스택을 GuestUISubsystem 에 등록.
 *
 * WBP_GuestPrimaryLayout 에서 이 클래스를 부모로 설정 후
 * 아래 이름으로 CommonActivatableWidgetStack 배치:
 *   Stack_Frontend / Stack_GameHUD / Stack_GameMenu / Stack_Modal
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class GUESTUI_API UGuestPrimaryLayout : public UCommonUserWidget
{
	GENERATED_BODY()

protected:

	//~ Begin UUserWidget Interface
	/* 위젯 초기화 시 각 스택을 GuestUISubsystem 에 등록 수행. */
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

private:

	// ── Widget Stack 슬롯 (BindWidget) ──

	/* 타이틀 / 메인메뉴 레이어. ZOrder 0. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetContainerBase> Stack_Frontend;

	/* 인게임 HUD 레이어. ZOrder 1. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetContainerBase> Stack_GameHUD;

	/* 인게임 메뉴 레이어. ZOrder 2. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetContainerBase> Stack_GameMenu;

	/* 모달 레이어. ZOrder 3 (최상단). */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetContainerBase> Stack_Modal;
};
