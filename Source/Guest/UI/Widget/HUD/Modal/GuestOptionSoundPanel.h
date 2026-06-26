// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GuestOptionSoundPanel.generated.h"

class USlider;
class UTextBlock;
class UCheckBox;
class UAkRtpc; // Wwise RTPC 클래스 전방 선언

UCLASS()
class GUEST_API UGuestOptionSoundPanel : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	// ── UI 컴포넌트 바인딩 ──
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_Master;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_MasterVol;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_BGM;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_BGMVol;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_SFX;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SFXVol;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_Ambience;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AmbienceVol;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> CheckBox_MuteBackground;

	// ── Wwise RTPC 에셋 바인딩 (에디터에서 할당) ──
	UPROPERTY(EditDefaultsOnly, Category = "Wwise|RTPC")
	TObjectPtr<UAkRtpc> RtpcAsset_Master;

	UPROPERTY(EditDefaultsOnly, Category = "Wwise|RTPC")
	TObjectPtr<UAkRtpc> RtpcAsset_BGM;

	UPROPERTY(EditDefaultsOnly, Category = "Wwise|RTPC")
	TObjectPtr<UAkRtpc> RtpcAsset_SFX;

	UPROPERTY(EditDefaultsOnly, Category = "Wwise|RTPC")
	TObjectPtr<UAkRtpc> RtpcAsset_Ambience;

	// ── 이벤트 핸들러 ──
	UFUNCTION() void OnMasterVolChanged(float Value);
	UFUNCTION() void OnBGMVolChanged(float Value);
	UFUNCTION() void OnSFXVolChanged(float Value);
	UFUNCTION() void OnAmbienceVolChanged(float Value);
	UFUNCTION() void OnMuteBackgroundChanged(bool bIsChecked);

	// ── 공통 RTPC 업데이트 헬퍼 (FName -> UAkRtpc* 로 변경) ──
	void UpdateVolumeRTPC(const UAkRtpc* RTPCAsset, float SliderValue, UTextBlock* TargetTextBlock);
};