// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h"
#include "Guest/UI/Types/GuestUITypes.h"
#include "GBarDialogueWidget.generated.h"

class UCommonTextBlock;
class UCommonButtonBase;
class ABarCustomerNPC;

/**
 * 바 모드 전체화면 대화 위젯 (VA-11 HALL-A 스타일).
 * GuestUISubsystem::OpenBarDialogue() 호출 시 BarDialogue 스택에 Push된다.
 * NativeOnActivated()에서 PendingBarDialogueData를 읽어 대화를 시작한다.
 *
 * 레이아웃 (WBP에서 구성):
 *  - Img_CharacterPortrait  : 화면 좌측, NPC 전신/반신 초상화
 *  - Text_SpeakerName       : 텍스트 박스 상단 화자 이름
 *  - Text_DialogueLine      : 텍스트 박스 본문 대사
 *  - Btn_Next               : 다음 대사 / 대화 종료 버튼
 */
UCLASS(Abstract)
class GUEST_API UGBarDialogueWidget : public UGuestActivatableBase
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
    void BlendToNPCCamera(ABarCustomerNPC* NPC);

    UFUNCTION()
    void OnNextClicked();

    FBarDialogueData CurrentBarData;
    int32 CurrentLineIndex = 0;
};
