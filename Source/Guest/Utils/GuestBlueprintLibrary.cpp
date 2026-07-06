// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Utils/GuestBlueprintLibrary.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

UGuestUISubsystem* UGuestBlueprintLibrary::GetGuestUISubsystem(const UObject* WorldContextObject)
{
    if (const UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
    {
        return GI->GetSubsystem<UGuestUISubsystem>();
    }
    return nullptr;
}
