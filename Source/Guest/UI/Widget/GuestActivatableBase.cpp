// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GuestActivatableBase.h"
#include "Guest/Core/Controllers/GuestPlayerController.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"

void UGuestActivatableBase::NativeOnActivated()
{
    Super::NativeOnActivated();
    UE_LOG(LogTemp, Verbose, TEXT("[GuestUI] 위젯 활성화: %s"), *GetName());
}

void UGuestActivatableBase::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();
    UE_LOG(LogTemp, Verbose, TEXT("[GuestUI] 위젯 비활성화: %s"), *GetName());
}

AGuestPlayerController* UGuestActivatableBase::GetGuestPlayerController() const
{
    // 유효한 캐시가 있으면 반환. 없으면 Cast 후 캐싱.
    if (!CachedController.IsValid())
    {
        CachedController = Cast<AGuestPlayerController>(GetOwningPlayer());
    }
    return CachedController.Get();
}

UGuestUISubsystem* UGuestActivatableBase::GetUISubsystem() const
{
    if (const AGuestPlayerController* PC = GetGuestPlayerController())
    {
        return PC->GetUISubsystem();
    }
    return nullptr;
}
