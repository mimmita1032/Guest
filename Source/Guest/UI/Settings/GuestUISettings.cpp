// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Settings/GuestUISettings.h"

UGuestUISettings::UGuestUISettings()
{
    CategoryName = TEXT("Game");
    SectionName  = TEXT("Guest UI Settings");
}

TSoftClassPtr<UUserWidget> UGuestUISettings::FindWidgetClassByTag(const FGameplayTag& Tag) const
{
    for (const FGuestWidgetMapping& Mapping : WidgetMappings)
    {
        if (Mapping.WidgetTag == Tag) return Mapping.WidgetClass;
    }
    UE_LOG(LogTemp, Warning, TEXT("[GuestUI] Tag '%s' 매핑된 위젯 클래스 없음."), *Tag.ToString());
    return nullptr;
}
