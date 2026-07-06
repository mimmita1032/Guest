// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Widget/GuestDialogueWidgetBase.h"
#include "GBarDialogueWidget.generated.h"

class ABarCustomerNPC;

/**
 * 바 모드 전체화면 대화 위젯 (VA-11 HALL-A 스타일).
 * GuestUISubsystem::OpenBarDialogue() 호출 시 BarDialogue 스택에 Push된다.
 * GuestDialogueWidgetBase의 공통 대화 로직에 NPC 카메라 블렌드를 추가한다.
 *
 * WBP 구성은 GuestDialogueWidgetBase 주석 참고.
 */
UCLASS(Abstract)
class GUEST_API UGBarDialogueWidget : public UGuestDialogueWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

private:
	void BlendToNPCCamera(ABarCustomerNPC* NPC);
};
