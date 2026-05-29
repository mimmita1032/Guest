// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/ButtonBase/GuestCommonButton.h"
#include "Guest/Sound/GuestAudioDataAsset.h"
#include "Guest/Sound/GuestSoundSubsystem.h"
#include "Guest/Sound/GuestSoundTags.h"

UGuestCommonButton::UGuestCommonButton()
{
	// 미리 정의해둔 UI 사운드 태그를 기본값으로 세팅.
	// WBP 에디터에서 태그를 재지정하면 버튼마다 다른 사운드 가능.
	HoverSoundTag = GuestSoundTags::TAG_Sound_Event_UI_ButtonHover;
	ClickSoundTag = GuestSoundTags::TAG_Sound_Event_UI_ButtonClick;
}

// ─────────────────────────────────────────────────────────
// UUserWidget 인터페이스
// ─────────────────────────────────────────────────────────

// NativeOnInitialized 대신 NativeConstruct 사용
void UGuestCommonButton::NativeConstruct()
{
	Super::NativeConstruct();

	OnClicked().AddUObject(this, &UGuestCommonButton::HandleClickSound);
}

// ─────────────────────────────────────────────────────────
// UCommonButtonBase 인터페이스
// ─────────────────────────────────────────────────────────

void UGuestCommonButton::NativeOnHovered()
{
	Super::NativeOnHovered();

	PlaySound(HoverSoundTag);
}

// ─────────────────────────────────────────────────────────
// 내부 헬퍼
// ─────────────────────────────────────────────────────────

void UGuestCommonButton::HandleClickSound()
{
	PlaySound(ClickSoundTag);
}

void UGuestCommonButton::PlaySound(const FGameplayTag& SoundTag)
{
	// AudioDataAsset 또는 태그가 없으면 스킵.
	if (!IsValid(AudioDataAsset) || !SoundTag.IsValid()) return;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGuestSoundSubsystem* SndSys = GI->GetSubsystem<UGuestSoundSubsystem>())
		{
			SndSys->PlayGlobalSound(SoundTag, AudioDataAsset);
		}
	}
}