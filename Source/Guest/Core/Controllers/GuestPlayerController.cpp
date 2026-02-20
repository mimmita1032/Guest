// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "Guest/Core/Controllers/GuestPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void AGuestPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	bShowMouseCursor = false;

	UE_LOG(LogTemp, Log, TEXT("플레이어 컨트롤러 초기화 및 입력 모드 설정 완료"));
}
