// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Core/GameModes/GuestGameModeBase.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Core/Controllers/GuestPlayerController.h"

AGuestGameModeBase::AGuestGameModeBase()
{
	
	DefaultPawnClass = AGuestCharacter::StaticClass();

	PlayerControllerClass = AGuestPlayerController::StaticClass();
	UE_LOG(LogTemp, Warning, TEXT("게임모드 프레임워크 설정 완료"));
	
}