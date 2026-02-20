// Fill out your copyright notice in the Description page of Project Settings.

#include "Guest/Core/GameModes/GuestGameModeBase.h"
#include "Guest/Characters/Player/GuestCharacter.h"

AGuestGameModeBase::AGuestGameModeBase()
{
	
DefaultPawnClass = AGuestCharacter::StaticClass();
	
}