// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Widget/GuestDialogueWidgetBase.h"
#include "GuestNPCDialogueWidget.generated.h"

/**
 * 스카이림 스타일 NPC 대화 위젯.
 * AGuestNPCBase::Interact_Implementation에서 열린다.
 * GameMenu 스택에 Push되며 카메라 전환 없이 인게임 HUD 위에 표시된다.
 *
 * WBP 구성은 GuestDialogueWidgetBase 주석 참고.
 */
UCLASS(Abstract)
class GUEST_API UGuestNPCDialogueWidget : public UGuestDialogueWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
};
