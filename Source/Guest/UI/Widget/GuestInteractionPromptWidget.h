// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Widget/GuestActivatableBase.h"
#include "GuestInteractionPromptWidget.generated.h"

/**
 * 상호작용 프롬프트 (예: [E] 사과 줍기) UI
 */
UCLASS(Abstract)
class GUEST_API UGuestInteractionPromptWidget : public UGuestActivatableBase
{
	GENERATED_BODY()

public:
	// 상호작용 컴포넌트(C++)에서 이 함수를 호출하면, 위젯 블루프린트(WBP)에서 텍스트를 갱신합니다.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	void UpdatePromptText(const FText& InteractText);
};
