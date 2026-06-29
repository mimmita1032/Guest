// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Widget_GameHUD.h"
#include "CommonUITypes.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Components/CharacterComponents/GuestPawnUIComponent.h"

void UWidget_GameHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn) return;

	AGuestCharacter* GuestChar = Cast<AGuestCharacter>(OwningPawn);
	if (!GuestChar) return;

	UGuestPawnUIComponent* PawnUI = GuestChar->GetPawnUIComponent();
	if (!PawnUI) return;

	PawnUI->OnHealthChanged.AddDynamic(this, &UWidget_GameHUD::HandleHealthChanged);
	PawnUI->OnBatteryChanged.AddDynamic(this, &UWidget_GameHUD::HandleBatteryChanged);

	// 구독 직후 현재 값을 즉시 수신하여 초기 상태 표시
	PawnUI->RequestCurrentValues();
}

TOptional<FUIInputConfig> UWidget_GameHUD::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently, true);
}

void UWidget_GameHUD::HandleHealthChanged(float Current, float Max)
{
	BP_OnHealthChanged(Current, Max);
}

void UWidget_GameHUD::HandleBatteryChanged(float Current, float Max)
{
	BP_OnBatteryChanged(Current, Max);
}
