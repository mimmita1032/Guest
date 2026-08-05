// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestGameHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Components/CharacterComponents/GuestPawnUIComponent.h"

void UGuestGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AGuestCharacter* GuestChar = Cast<AGuestCharacter>(GetOwningPlayerPawn()))
	{
		if (UGuestPawnUIComponent* PawnUIComp = GuestChar->GetPawnUIComponent())
		{
			// 1. 델리게이트에 C++ 업데이트 함수 바인딩
			PawnUIComp->OnHealthChanged.AddDynamic(this, &UGuestGameHUDWidget::UpdateHealthUI);
			PawnUIComp->OnBatteryChanged.AddDynamic(this, &UGuestGameHUDWidget::UpdateBatteryUI);

			// 2. ★ 중요: 바인딩 직후, 현재 체력과 배터리 값을 즉시 UI에 반영하도록 요청
			PawnUIComp->RequestCurrentValues();
		}
	}
}
void UGuestGameHUDWidget::UpdateHealthUI(float CurrentHealth, float MaxHealth)
{
	if (MaxHealth > 0.f && PB_Health)
	{
		PB_Health->SetPercent(CurrentHealth / MaxHealth);
	}
	
	if (Text_Health)
	{
		// 실수를 정수로 반올림하여 텍스트 표시 (예: 60)
		Text_Health->SetText(FText::AsNumber(FMath::RoundToInt(CurrentHealth)));
	}
}

void UGuestGameHUDWidget::UpdateBatteryUI(float CurrentBattery, float MaxBattery)
{
	if (MaxBattery > 0.f && PB_Battery)
	{
		PB_Battery->SetPercent(CurrentBattery / MaxBattery);
	}
	
	if (Text_Battery)
	{
		Text_Battery->SetText(FText::AsNumber(FMath::RoundToInt(CurrentBattery)));
	}
}