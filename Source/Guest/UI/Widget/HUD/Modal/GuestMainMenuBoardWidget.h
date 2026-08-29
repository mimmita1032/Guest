// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/SaveLoad/GuestSaveSlotBoardBase.h" // 다른 파트 분이 만든 베이스 클래스 인클루드
#include "GuestMainMenuBoardWidget.generated.h"

UCLASS()
class GUEST_API UGuestMainMenuBoardWidget : public UGuestSaveSlotBoardBase
{
    GENERATED_BODY()
    
protected:
    // 베이스 클래스의 함수 오버라이드
    virtual void OnSlotWidgetCreated(UGuestSaveSlotWidget* SlotWidget, int32 SlotIndex) override;
    
    // 이 클래스만의 클릭 핸들러
    UFUNCTION()
    void HandleMainMenuSlotClicked(int32 SlotIndex);

private:
    void PrepareForGameStart();
};