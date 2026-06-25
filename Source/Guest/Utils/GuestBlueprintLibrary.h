// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GuestBlueprintLibrary.generated.h"

class UGuestUISubsystem;

UCLASS()
class GUEST_API UGuestBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Guest|UI", meta = (WorldContext = "WorldContextObject", CompactNodeTitle = "GuestUI"))
    static UGuestUISubsystem* GetGuestUISubsystem(const UObject* WorldContextObject);
};
