// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h"
#include "GuestDemoEndWidget.generated.h"

class UGuestCommonButton;
class UCommonTextBlock;

/**
 * UGuestDemoEndWidget
 *
 * 데모 종료 화면. 엔딩 나레이션이 걷힌 뒤 표시된다.
 *
 * 제작진 크레딧은 넣지 않는다 — "데모가 여기서 끝났다"를 전하는 것이 전부다.
 * 본편에서 크레딧이 필요해지면 이 위젯이 아니라 별도 화면으로 만든다.
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class GUEST_API UGuestDemoEndWidget : public UGuestActivatableBase
{
	GENERATED_BODY()

protected:

	//~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

private:

	// 종료 문구. WBP에서 내용을 채운다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> Text_Message;

	// 메인 메뉴로 — 아직 메뉴가 완성되지 않았다면 WBP에서 빼도 된다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UGuestCommonButton> Button_MainMenu;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UGuestCommonButton> Button_Quit;

	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void OnQuitClicked();
};
