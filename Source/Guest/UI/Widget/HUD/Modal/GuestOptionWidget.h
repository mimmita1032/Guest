// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h"
#include "GuestOptionWidget.generated.h"

class UGuestTabList;
class UCommonAnimatedSwitcher;
class UGuestCommonButton;

UCLASS()
class GUEST_API UGuestOptionWidget : public UGuestActivatableBase
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnActivated() override;
	
private:

	// ── 탭 시스템 컴포넌트 ──
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGuestTabList> TabList_Menu; // 새로 만든 C++ 클래스로 변경

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonAnimatedSwitcher> Switcher_Content;

	UPROPERTY(EditDefaultsOnly, Category = "Guest|Tab")
	TSubclassOf<UGuestCommonButton> TabButtonClass;

	// ── 탭 내용물 패널 ──
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> Panel_GamePlay; 

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> Panel_Graphics; 

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> Panel_Sound;    

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> Panel_Controls; 

	// ── 하단 액션 버튼 ──
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGuestCommonButton> Button_Quit;

	// ── 핸들러 ──
	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void OnTabSelected(FName TabId);
};