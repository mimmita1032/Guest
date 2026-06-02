// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Widget/GuestActivatableBase.h"
#include "Guest/UI/Types/GuestUITypes.h"
#include "GuestNPCDialogueWidget.generated.h"

class UCommonTextBlock;
class UCommonButtonBase;

/**
 * NPC 대화 위젯.
 * UGuestUISubsystem::OpenNPCDialogue() 호출 시 GameMenu 스택에 Push된다.
 * NativeOnActivated()에서 Subsystem의 PendingDialogueData를 읽어 대화를 시작한다.
 */
UCLASS(Abstract)
class GUEST_API UGuestNPCDialogueWidget : public UGuestActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Text_SpeakerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Text_DialogueLine;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> Btn_Next;

	void ShowCurrentLine();

	UFUNCTION()
	void OnNextClicked();

	FNPCDialogueData CurrentDialogueData;
	int32 CurrentLineIndex = 0;
};
