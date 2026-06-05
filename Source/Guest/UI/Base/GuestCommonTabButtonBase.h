// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestCommonButton.h"
#include "GuestCommonTabButtonBase.generated.h"

class UCommonTextBlock;

UCLASS()
class GUEST_API UGuestCommonTabButtonBase: public UGuestCommonButton
{
	GENERATED_BODY()

public:
	// C++에서 탭 이름을 즉시 변경하는 함수
	void SetTabName(const FText& InTabName);

protected:
	// WBP에서 배치할 텍스트 컴포넌트 (이름을 반드시 Text_TabName 으로 맞출 것)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Text_TabName; 
};