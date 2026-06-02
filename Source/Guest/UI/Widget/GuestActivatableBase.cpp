// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GuestActivatableBase.h"
#include "Guest/Core/Controllers/GuestPlayerController.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/Sound/GuestSoundSubsystem.h"
#include "Guest/Sound/GuestSoundTags.h"
#include "Guest/Sound/GuestAudioDataAsset.h"

// ─────────────────────────────────────────────────────────
// UCommonActivatableWidget 인터페이스
// ─────────────────────────────────────────────────────────

void UGuestActivatableBase::NativeOnActivated()
{
    Super::NativeOnActivated();

    UE_LOG(LogTemp, Verbose, TEXT("[GuestUI] 위젯 활성화: %s"), *GetName());

    // 1. [Open 사운드 자동 재생]
    // AudioData 가 설정된 위젯만 사운드 재생. 비어있으면 스킵.
    // DA_GuestSound AudioMap 에 TAG_Sound_Event_UI_ScreenOpen 이 등록되어 있어야 함.
    if (IsValid(AudioData))
    {
        if (UGuestSoundSubsystem* SndSys = GetSoundSubsystem())
        {
            SndSys->PlayGlobalSound(
                GuestSoundTags::TAG_Sound_Event_UI_ScreenOpen,
                AudioData
            );
        }
    }
}

void UGuestActivatableBase::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();

    UE_LOG(LogTemp, Verbose, TEXT("[GuestUI] 위젯 비활성화: %s"), *GetName());

    // 2. [Close 사운드 자동 재생]
    // AudioData 가 설정된 위젯만 사운드 재생. 비어있으면 스킵.
    // DA_GuestSound AudioMap 에 TAG_Sound_Event_UI_ScreenClose 가 등록되어 있어야 함.
    if (IsValid(AudioData))
    {
        if (UGuestSoundSubsystem* SndSys = GetSoundSubsystem())
        {
            SndSys->PlayGlobalSound(
                GuestSoundTags::TAG_Sound_Event_UI_ScreenClose,
                AudioData
            );
        }
    }
}

// ─────────────────────────────────────────────────────────
// 접근자
// ─────────────────────────────────────────────────────────

AGuestPlayerController* UGuestActivatableBase::GetGuestPlayerController() const
{
    // 유효한 캐시가 있으면 반환하여 Cast 반복 수행 방지.
    if (!CachedController.IsValid())
    {
        CachedController = Cast<AGuestPlayerController>(GetOwningPlayer());
    }
    return CachedController.Get();
}

UGuestUISubsystem* UGuestActivatableBase::GetUISubsystem() const
{
    if (const AGuestPlayerController* PC = GetGuestPlayerController())
    {
        return PC->GetUISubsystem();
    }
    return nullptr;
}

UGuestSoundSubsystem* UGuestActivatableBase::GetSoundSubsystem() const
{
    // PlayerController 를 통해 GameInstance 에 접근하여 SoundSubsystem 반환.
    if (const AGuestPlayerController* PC = GetGuestPlayerController())
    {
        if (UGameInstance* GI = PC->GetGameInstance())
        {
            return GI->GetSubsystem<UGuestSoundSubsystem>();
        }
    }
    return nullptr;
}