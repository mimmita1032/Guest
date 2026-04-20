// GuestSFXComponent.h
#pragma once

#include "AkComponent.h"
#include "GameplayTagContainer.h"
#include "GuestSFXComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUEST_API UGuestSFXComponent : public UAkComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Guest|Audio")
    void PlaySFX(FGameplayTag Tag, const UGuestAudioDataAsset* DataAsset);
};

