// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Core/GameModes/GuestGameModeBase.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Core/Controllers/GuestPlayerController.h"
#include "Guest/Core/States/GuestGameState.h"
#include "Guest/Core/States/GuestPlayerState.h"
#include "Guest/Subsystem/GSpacetimeSubsystem.h"

AGuestGameModeBase::AGuestGameModeBase()
{
	
	DefaultPawnClass = AGuestCharacter::StaticClass();
	PlayerControllerClass = AGuestPlayerController::StaticClass();
	PlayerStateClass = AGuestPlayerState::StaticClass();
	GameStateClass = AGuestGameState::StaticClass();

#pragma region TimeTick
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
#pragma endregion // TimeTick
}


#pragma region TimeTick
void AGuestGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGSpacetimeSubsystem* SpacetimeSS = GI->GetSubsystem<UGSpacetimeSubsystem>())
		{
			SpacetimeSS->UpdateWorldTime(DeltaTime);
		}
	}
}
#pragma endregion