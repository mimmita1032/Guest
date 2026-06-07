// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestOptionSoundPanel.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
#include "AkGameplayStatics.h"
#include "AkRtpc.h" // UAkRtpc 사용을 위해 필수 포함

void UGuestOptionSoundPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (Slider_Master)   Slider_Master->OnValueChanged.AddDynamic(this, &ThisClass::OnMasterVolChanged);
	if (Slider_BGM)      Slider_BGM->OnValueChanged.AddDynamic(this, &ThisClass::OnBGMVolChanged);
	if (Slider_SFX)      Slider_SFX->OnValueChanged.AddDynamic(this, &ThisClass::OnSFXVolChanged);
	if (Slider_Ambience) Slider_Ambience->OnValueChanged.AddDynamic(this, &ThisClass::OnAmbienceVolChanged);
	if (CheckBox_MuteBackground) CheckBox_MuteBackground->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnMuteBackgroundChanged);
}

void UGuestOptionSoundPanel::UpdateVolumeRTPC(const UAkRtpc* RTPCAsset, float SliderValue, UTextBlock* TargetTextBlock)
{
	// 에셋이 에디터에서 할당되지 않았다면 안전하게 무시
	if (!RTPCAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[OptionSoundPanel] RTPC 에셋이 할당되지 않았습니다!"));
		return;
	}

	float RTPCValue = SliderValue * 100.0f;

	if (TargetTextBlock)
	{
		FString VolString = FString::Printf(TEXT("%d%%"), FMath::RoundToInt(RTPCValue));
		TargetTextBlock->SetText(FText::FromString(VolString));
	}

	// Wwise 전역(Global) RTPC 적용
	UAkGameplayStatics::SetRTPCValue(RTPCAsset, RTPCValue, 0, nullptr);
}

void UGuestOptionSoundPanel::OnMasterVolChanged(float Value)
{
	UpdateVolumeRTPC(RtpcAsset_Master, Value, Text_MasterVol);
}

void UGuestOptionSoundPanel::OnBGMVolChanged(float Value)
{
	UpdateVolumeRTPC(RtpcAsset_BGM, Value, Text_BGMVol);
}

void UGuestOptionSoundPanel::OnSFXVolChanged(float Value)
{
	UpdateVolumeRTPC(RtpcAsset_SFX, Value, Text_SFXVol);
}

void UGuestOptionSoundPanel::OnAmbienceVolChanged(float Value)
{
	UpdateVolumeRTPC(RtpcAsset_Ambience, Value, Text_AmbienceVol);
}

void UGuestOptionSoundPanel::OnMuteBackgroundChanged(bool bIsChecked)
{
	FApp::SetUnfocusedVolumeMultiplier(bIsChecked ? 0.0f : 1.0f);
}