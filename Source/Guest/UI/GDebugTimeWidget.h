// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GDebugTimeWidget.generated.h"

class USlider;
class UTextBlock;

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

protected:
	// 슬라이더와 이름을 맞춰 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> TimeSlider;

	// 블루프린트 위젯의 텍스트와 이름을 맞춰 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimeText;
};
