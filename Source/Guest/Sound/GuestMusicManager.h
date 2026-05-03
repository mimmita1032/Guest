#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GameplayTagContainer.h"
#include "GuestMusicManager.generated.h"

class UAkAudioEvent;
class UGuestAudioDataAsset;

UCLASS(BlueprintType)
class GUEST_API AGuestMusicManager : public AInfo
{
    GENERATED_BODY()

public:
    AGuestMusicManager();

    /** * 태그를 입력받아 해당 음악으로 전환합니다. 
     * @param Tag 재생할 사운드 태그 (Sound.BGM.xxx)
     * @param DataAsset 사운드 정보가 담긴 데이터 에셋
     */
    UFUNCTION(BlueprintCallable, Category = "Guest|Audio")
    void TransitionMusic(FGameplayTag Tag, const UGuestAudioDataAsset* DataAsset);

    /** 현재 재생 중인 음악을 중지합니다. */
    UFUNCTION(BlueprintCallable, Category = "Guest|Audio")
    void StopMusic();

protected:
    // 현재 재생 중인 이벤트를 저장하여 중복 재생을 방지합니다.
    UPROPERTY(VisibleInstanceOnly, Category = "Guest|Audio")
    TObjectPtr<UAkAudioEvent> CurrentMusicEvent;

    // 마지막으로 재생된 PlayingID (중지 시 필요할 수 있음)
    int32 CurrentPlayingID;
};