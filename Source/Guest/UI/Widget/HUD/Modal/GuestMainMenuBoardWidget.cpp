// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestMainMenuBoardWidget.h"
#include "Guest/UI/SaveLoad/GuestSaveSlotWidget.h"
#include "Guest/Save/GuestSaveSlotNames.h"
#include "Guest/Save/GuestSaveGame.h"
#include "Guest/Core/GameInstance/GuestGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UGuestMainMenuBoardWidget::OnSlotWidgetCreated(UGuestSaveSlotWidget* SlotWidget, int32 SlotIndex)
{
    if (!SlotWidget) return;
    
    // 다른 파트 분이 만든 슬롯 위젯의 델리게이트에 내 함수를 연결합니다.
    SlotWidget->OnSlotClicked.AddDynamic(this, &UGuestMainMenuBoardWidget::HandleMainMenuSlotClicked);
}

void UGuestMainMenuBoardWidget::HandleMainMenuSlotClicked(int32 InSlotIndex)
{
    const FString SlotName = GuestSaveSlots::MakeSlotName(InSlotIndex);
    const int32 UserIndex = GuestSaveSlots::DefaultUserIndex();

    // 1. 세이브 데이터 존재 여부 검사
    if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
    {
        // [이어하기 로직] - 기존 LoadBoardWidget과 동일한 방식 사용
        if (UGuestGameInstance* GI = Cast<UGuestGameInstance>(GetGameInstance()))
        {
            PrepareForGameStart();
            GI->RequestLoadFromSlot(SlotName, UserIndex);
        }
    }
    else
    {
        // [새로운 게임 로직] - 덮어쓰기 후 강제 이동
        if (UGuestSaveGame* NewSave = Cast<UGuestSaveGame>(UGameplayStatics::CreateSaveGameObject(UGuestSaveGame::StaticClass())))
        {
            NewSave->MapPackageName = TEXT("/Game/maps/L_Field_01"); // 데모 시작점
            NewSave->SavedStoryProgress = 0;
            NewSave->SavedAt = FDateTime::Now();

            UGameplayStatics::SaveGameToSlot(NewSave, SlotName, UserIndex);
            
            PrepareForGameStart();
            UGameplayStatics::OpenLevel(this, FName("L_Field_01"));
        }
    }
}

void UGuestMainMenuBoardWidget::PrepareForGameStart()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
}