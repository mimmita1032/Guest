// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "GuestTabList.generated.h"

class UHorizontalBox;

UCLASS()
class GUEST_API UGuestTabList : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

protected:
	// 탭 버튼들이 동적으로 들어갈 가로 상자 (이름을 반드시 HBox_Tabs 로 맞출 것)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HBox_Tabs;

	// 핵심: BP의 'Event Handle Tab Creation' 노드를 C++로 완전히 대체
	virtual void HandleTabCreation_Implementation(FName TabNameID, UCommonButtonBase* TabButton) override;
};