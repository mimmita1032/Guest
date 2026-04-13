// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GuestActivatableBase.h"
#include "Guest/Core/Controllers/GuestPlayerController.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"

// ─────────────────────────────────────────────────────────
// UCommonActivatableWidget 인터페이스
// ─────────────────────────────────────────────────────────

void UGuestActivatableBase::NativeOnActivated()
{
    Super::NativeOnActivated();

    // Verbose 레벨 로그로 출력하여 일반 플레이 중 콘솔 오염 최소화.
    // 디버깅 시 log LogTemp Verbose 명령으로 확인 가능.
    UE_LOG(LogTemp, Verbose, TEXT("[GuestUI] 위젯 활성화: %s"), *GetName());
}

void UGuestActivatableBase::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();

    UE_LOG(LogTemp, Verbose, TEXT("[GuestUI] 위젯 비활성화: %s"), *GetName());
}

// ─────────────────────────────────────────────────────────
// 접근자
// ─────────────────────────────────────────────────────────

AGuestPlayerController* UGuestActivatableBase::GetGuestPlayerController() const
{
    // 유효한 캐시가 있으면 반환하여 Cast 반복 수행 방지.
    // 캐시가 없거나 무효화된 경우에만 새로 Cast 하여 캐싱.
    if (!CachedController.IsValid())
    {
        CachedController = Cast<AGuestPlayerController>(GetOwningPlayer());
    }
    return CachedController.Get();
}

UGuestUISubsystem* UGuestActivatableBase::GetUISubsystem() const
{
    // PlayerController 를 통해 GameInstance Subsystem 에 접근.
    // PlayerController 가 없으면 nullptr 반환.
    if (const AGuestPlayerController* PC = GetGuestPlayerController())
    {
        return PC->GetUISubsystem();
    }
    return nullptr;
}
