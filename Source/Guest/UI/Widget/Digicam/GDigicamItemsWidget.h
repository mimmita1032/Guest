// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GDigicamTabBase.h"
#include "GDigicamItemsWidget.generated.h"

class UGuestUISubsystem;

// 디지캠 Items 탭 — 탭 진입 시 인벤토리 화면을 그대로 띄움 (I키로 여는 것과 동일한 경로)
UCLASS(Abstract)
class GUEST_API UGDigicamItemsWidget : public UGDigicamTabBase
{
	GENERATED_BODY()

public:
	virtual void OnTabActivated_Implementation() override;

private:
	UGuestUISubsystem* GetUISubsystem() const;
};
