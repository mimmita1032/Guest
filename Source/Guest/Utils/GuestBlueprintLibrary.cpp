// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Utils/GuestBlueprintLibrary.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Components/TextWidgetTypes.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

UGuestUISubsystem* UGuestBlueprintLibrary::GetGuestUISubsystem(const UObject* WorldContextObject)
{
    if (const UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
    {
        return GI->GetSubsystem<UGuestUISubsystem>();
    }
    return nullptr;
}

void UGuestBlueprintLibrary::ApplyAutoWrap(UTextLayoutWidget* TextWidget, float WrapTextAt)
{
    if (!TextWidget)
    {
        return;
    }

    // 두 세터 모두 값이 실제로 바뀔 때만 Slate에 반영한다 —
    // WBP에서 이미 켜둔 위젯에 다시 불러도 손해가 없다.
    TextWidget->SetAutoWrapText(true);
    TextWidget->SetWrapTextAt(FMath::Max(WrapTextAt, 0.f));
}
