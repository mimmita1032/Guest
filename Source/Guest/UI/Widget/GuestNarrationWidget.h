// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Widget/GuestNarrationWidgetBase.h"
#include "GuestNarrationWidget.generated.h"

/**
 * 퀘스트 완료 등 스토리 분기 전환 나레이션 위젯.
 * GQuestSubsystem::OnNarrationRequested → GuestUISubsystem::OpenNarration에서 열린다.
 * Narration 스택에 Push되며 전체화면으로 표시된다.
 *
 * WBP 구성은 GuestNarrationWidgetBase 주석 참고.
 */
UCLASS(Abstract)
class GUEST_API UGuestNarrationWidget : public UGuestNarrationWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
};
