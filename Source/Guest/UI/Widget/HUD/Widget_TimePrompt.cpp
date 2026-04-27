// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/HUD/Widget_TimePrompt.h"
#include "Guest/Subsystem/GSpacetimeSubsystem.h"

void UWidget_TimePrompt::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UGSpacetimeSubsystem* SpacetimeSys = GameInstance->GetSubsystem<UGSpacetimeSubsystem>())
		{
			SpacetimeSys->OnTimeChanged.AddDynamic(this, &UWidget_TimePrompt::OnTimeChanged);
			
			OnTimeChanged(SpacetimeSys->GetCurrentHour());
		}
	}
}

void UWidget_TimePrompt::OnTimeChanged(float CurrentHour)
{
	// float 시간(예: 9.5)을 시(9)와 분(30)으로 나눔
	int32 Hours = FMath::FloorToInt(CurrentHour);
	int32 Minutes = FMath::FloorToInt((CurrentHour - Hours) * 60.0f);

	// "09:30" 형식으로 텍스트 나오게
	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);
	UpdateTimeText(FText::FromString(TimeString));
}