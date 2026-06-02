// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "GameplayTagContainer.h"
#include "GuestCommonButton.generated.h"

class UGuestAudioDataAsset;

/**
 * UGuestCommonButton
 *
 * Wwise 사운드 시스템이 연동된 CommonUI 기본 버튼 클래스.
 * NativeOnHovered → Hover 사운드 재생.
 * NativeOnClicked → Click 사운드 재생.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class GUEST_API UGuestCommonButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:

	UGuestCommonButton();

protected:

	//~ Begin UCommonButtonBase Interface
	/** Hover 사운드 재생. */
	virtual void NativeOnHovered() override;
    
	/** Click 사운드 재생. */
	virtual void NativeOnClicked() override;
	//~ End UCommonButtonBase Interface

protected:

	/** 버튼 사운드 설정이 담긴 데이터 에셋. WBP 에디터에서 할당. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guest|Audio")
	TObjectPtr<UGuestAudioDataAsset> AudioDataAsset;

	/** Hover 시 재생할 사운드 태그. 기본값: TAG_Sound_Event_UI_ButtonHover. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guest|Audio")
	FGameplayTag HoverSoundTag;

	/** Click 시 재생할 사운드 태그. 기본값: TAG_Sound_Event_UI_ButtonClick. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guest|Audio")
	FGameplayTag ClickSoundTag;

private:

	/** 공통 사운드 재생 헬퍼. */
	void PlaySound(const FGameplayTag& SoundTag);
};