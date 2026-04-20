// GuestSoundSubsystem.cpp
#include "GuestSoundSubsystem.h"
#include "GuestAudioDataAsset.h"
#include "AkGameplayStatics.h"

void UGuestSoundSubsystem::PlayGlobalSound(FGameplayTag Tag, const UGuestAudioDataAsset* DataAsset)
{
    if (!DataAsset) return;

    if (const FGuestAudioAction* Action = DataAsset->FindAction(Tag))
    {
        // 1. State 설정 (UAkGameplayStatics 소스코드 시그니처 준수)
        for (UAkStateValue* State : Action->States)
        {
            if (State)
            {
                // 소스코드 확인 결과: SetState는 State 객체 하나만 인자로 받습니다.
                UAkGameplayStatics::SetState(State);
            }
        }

        // 2. Global RTPC 설정
        for (const auto& RTPCPair : Action->RTPCs)
        {
            if (RTPCPair.Key)
            {
                UAkGameplayStatics::SetRTPCValue(RTPCPair.Key, RTPCPair.Value, 0, nullptr);
            }
        }

        // 3. UI/Global Event 실행
        if (Action->Event)
        {
            UAkGameplayStatics::PostEvent(Action->Event, nullptr, 0, FOnAkPostEventCallback(), false);
        }
    }
}