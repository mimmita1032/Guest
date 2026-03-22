// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GDebugTimeWidget.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Guest/Subsystem/GSpacetimeSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UGDebugTimeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TimeSlider)
	{
		TimeSlider->SetMinValue(0.0f);
		TimeSlider->SetMaxValue(23.99f);
		TimeSlider->OnValueChanged.AddUniqueDynamic(this, &UGDebugTimeWidget::OnTimeSliderChanged);	}

	if (SyncButton)
	{
		SyncButton->OnClicked.AddUniqueDynamic(this, &UGDebugTimeWidget::OnSyncButtonClicked);
	}
	
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGSpacetimeSubsystem* SpacetimeSS = GI->GetSubsystem<UGSpacetimeSubsystem>())
		{
			SpacetimeSS->OnTimeChanged.AddUniqueDynamic(this, &UGDebugTimeWidget::UpdateTimeText);
			
			// 생성 직후 현재 시간으로 텍스트 1회 초기화
			UpdateTimeText(SpacetimeSS->GetCurrentHour());
		}
	}
}

void UGDebugTimeWidget::OnTimeSliderChanged(float Value)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGSpacetimeSubsystem* SpacetimeSS = GI->GetSubsystem<UGSpacetimeSubsystem>())
		{
			SpacetimeSS->SetWorldTime(Value);
		}
	}
}

void UGDebugTimeWidget::UpdateTimeText(float CurrentHour)
{
	if (TimeText)
	{
		FString TimeString = FString::Printf(TEXT("디버그 시간: %.2f시"), CurrentHour);
		TimeText->SetText(FText::FromString(TimeString));
	}
}


void UGDebugTimeWidget::OnSyncButtonClicked()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGSpacetimeSubsystem* SpacetimeSS = GI->GetSubsystem<UGSpacetimeSubsystem>())
		{
			SpacetimeSS->ResumeRealTimeSync();
		}
	}
}