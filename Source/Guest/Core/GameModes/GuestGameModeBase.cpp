// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Core/GameModes/GuestGameModeBase.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Core/Controllers/GuestPlayerController.h"
#include "Guest/Core/States/GuestGameState.h"
#include "Guest/Core/States/GuestPlayerState.h"

AGuestGameModeBase::AGuestGameModeBase()
{
	
	DefaultPawnClass = AGuestCharacter::StaticClass();

	PlayerControllerClass = AGuestPlayerController::StaticClass();

	PlayerStateClass = AGuestPlayerState::StaticClass();

	GameStateClass = AGuestGameState::StaticClass();	
}
