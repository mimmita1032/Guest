// GuestSoundSubsystem.h
#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "GuestSoundSubsystem.generated.h"

UCLASS()
class GUEST_API UGuestSoundSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // 전역 사운드 실행 (State & Global RTPC 위주)
    UFUNCTION(BlueprintCallable, Category = "Guest|Audio")
    void PlayGlobalSound(FGameplayTag Tag, const UGuestAudioDataAsset* DataAsset);
};

