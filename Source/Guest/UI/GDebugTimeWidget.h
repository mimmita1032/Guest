// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GDebugTimeWidget.generated.h"

class USlider;
class UTextBlock;
class UButton;

UCLASS()
class GUEST_API UGDebugTimeWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// 슬라이더 값을 조절할 때 호출
	UFUNCTION()
	void OnTimeSliderChanged(float Value);

	// 시간이 변할 때 텍스트를 갱신
	UFUNCTION()
	void UpdateTimeText(float CurrentHour);

	// 동기화 버튼
	UFUNCTION()
	void OnSyncButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> TimeSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimeText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SyncButton;
};
