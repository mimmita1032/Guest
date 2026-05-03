#include "GuestMusicManager.h"
#include "GuestAudioDataAsset.h"
#include "AkGameplayStatics.h"
#include "AkAudioEvent.h"

AGuestMusicManager::AGuestMusicManager()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentPlayingID = 0;
}

void AGuestMusicManager::TransitionMusic(FGameplayTag Tag, const UGuestAudioDataAsset* DataAsset)
{
    if (!DataAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MusicManager] DataAsset is NULL"));
        return;
    }

    const FGuestAudioAction* Action = DataAsset->FindAction(Tag);
    if (!Action) return;

    // 1. State 전환 (음악 장르나 분위기 변경)
    // UAkGameplayStatics::SetState(const UAkStateValue*) 시그니처 사용
    for (UAkStateValue* State : Action->States)
    {
        if (State)
        {
            UAkGameplayStatics::SetState(State);
        }
    }

    // 2. RTPC 설정 (음악의 긴박함 등 제어)
    for (const auto& RTPCPair : Action->RTPCs)
    {
        if (RTPCPair.Key)
        {
            UAkGameplayStatics::SetRTPCValue(RTPCPair.Key, RTPCPair.Value, 0, this);
        }
    }

    // 3. 음악 이벤트 재생 (중복 재생 방지)
    if (Action->Event && Action->Event != CurrentMusicEvent)
    {
        // UAkGameplayStatics::PostEvent (인자 5개 버전)
        // AkEvent, Actor, CallbackMask, PostEventCallback, bStopWhenAttachedToDestroyed
        CurrentPlayingID = UAkGameplayStatics::PostEvent(
            Action->Event, 
            this, 
            0, 
            FOnAkPostEventCallback(), 
            false
        );

        CurrentMusicEvent = Action->Event;
    }
}

void AGuestMusicManager::StopMusic()
{
    if (CurrentMusicEvent)
    {
        // 현재 액터(this)에서 재생 중인 모든 사운드 중지 또는 특정 이벤트 중지
        UAkGameplayStatics::PostEvent(CurrentMusicEvent, this, 0, FOnAkPostEventCallback(), true);
        CurrentMusicEvent = nullptr;
        CurrentPlayingID = 0;
    }
}