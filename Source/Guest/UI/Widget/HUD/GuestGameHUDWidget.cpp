// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestGameHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Components/CharacterComponents/GuestPawnUIComponent.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "Guest/Items/Instance/GItemInstance.h"

void UGuestGameHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (AGuestCharacter* GuestChar = Cast<AGuestCharacter>(GetOwningPlayerPawn()))
    {
       if (UGuestPawnUIComponent* PawnUIComp = GuestChar->GetPawnUIComponent())
       {
          // 1. 델리게이트에 C++ 업데이트 함수 바인딩
          PawnUIComp->OnHealthChanged.AddDynamic(this, &UGuestGameHUDWidget::UpdateHealthUI);
          PawnUIComp->OnBatteryChanged.AddDynamic(this, &UGuestGameHUDWidget::UpdateBatteryUI);

          // 2. 바인딩 직후, 현재 체력과 배터리 값을 즉시 UI에 반영하도록 요청
          PawnUIComp->RequestCurrentValues();
       }

       // 3. 인벤토리 퀵슬롯 델리게이트 바인딩
       if (UGInventoryComponent* InvComp = GuestChar->FindComponentByClass<UGInventoryComponent>())
       {
          InvComp->OnInventoryChanged.AddDynamic(this, &UGuestGameHUDWidget::UpdateQuickSlotsUI);
          
          // HUD가 켜질 때 퀵슬롯 아이콘도 최초 1회 즉시 갱신
          UpdateQuickSlotsUI();
       }
    }
}

void UGuestGameHUDWidget::UpdateHealthUI(float CurrentHealth, float MaxHealth)
{
    if (MaxHealth > 0.f && PB_Health)
    {
       PB_Health->SetPercent(CurrentHealth / MaxHealth);
    }
    
    if (Text_Health)
    {
       Text_Health->SetText(FText::AsNumber(FMath::RoundToInt(CurrentHealth)));
    }
}

void UGuestGameHUDWidget::UpdateBatteryUI(float CurrentBattery, float MaxBattery)
{
    if (MaxBattery > 0.f && PB_Battery)
    {
       PB_Battery->SetPercent(CurrentBattery / MaxBattery);
    }
    
    if (Text_Battery)
    {
       Text_Battery->SetText(FText::AsNumber(FMath::RoundToInt(CurrentBattery)));
    }
}

void UGuestGameHUDWidget::UpdateQuickSlotsUI()
{
    AGuestCharacter* GuestChar = Cast<AGuestCharacter>(GetOwningPlayerPawn());
    if (!GuestChar) return;

    UGInventoryComponent* InvComp = GuestChar->FindComponentByClass<UGInventoryComponent>();
    if (!InvComp) return;

    // UI 이미지 배열화 (4칸으로 복구)
    TArray<UImage*> QuickSlotImages = { Img_QuickSlot_0, Img_QuickSlot_1, Img_QuickSlot_2, Img_QuickSlot_3 };

    // 반복 횟수 4로 수정
    for (int32 i = 0; i < 4; ++i)
    {
        UImage* TargetImage = QuickSlotImages[i];
        if (!TargetImage) continue;

        FInventoryItemHandle QuickHandle = InvComp->GetQuickSlotItem(i);
        TSoftObjectPtr<UTexture2D> IconToSet = nullptr;

        if (QuickHandle.IsValid())
        {
            if (UGItemInstance* ItemInst = InvComp->GetItemByHandle(QuickHandle))
            {
                if (const UGItemFragmentInventory* InvFrag = ItemInst->FindFragmentByClass<UGItemFragmentInventory>())
                {
                    IconToSet = InvFrag->ItemIcon;
                }
            }
        }

        // 아이콘 적용 및 가시성 설정
        if (IconToSet.IsValid() || IconToSet.IsPending())
        {
            TargetImage->SetBrushFromSoftTexture(IconToSet);
            TargetImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        }
        else
        {
            TargetImage->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}