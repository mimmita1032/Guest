// GuestSFXComponent.cpp
#include "GuestSFXComponent.h"
#include "GuestAudioDataAsset.h"
#include "AkGameplayStatics.h"

void UGuestSFXComponent::PlaySFX(FGameplayTag Tag, const UGuestAudioDataAsset* DataAsset)
{
    if (!DataAsset) return;

    if (const FGuestAudioAction* Action = DataAsset->FindAction(Tag))
    {
        // 1. Switch 설정 (UAkGameplayStatics 사용)
        for (UAkSwitchValue* Switch : Action->Switches)
        {
            if (Switch) UAkGameplayStatics::SetSwitch(Switch, GetOwner());
        }

        // 2. RTPC 설정 (UAkComponent 멤버 함수 사용 가능)
        for (const auto& RTPCPair : Action->RTPCs)
        {
            if (RTPCPair.Key) SetRTPCValue(RTPCPair.Key, RTPCPair.Value, 0, "");
        }

        // 3. Event 실행 (UAkComponent 내부 public 메서드 사용)
        if (Action->Event)
        {
            // 제시해주신 3개 인자 시그니처에 맞춤
            PostAkEvent(
                Action->Event, 
                0, 
                FOnAkPostEventCallback()
            );
        }
    }
}