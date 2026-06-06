// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GuestUIInputConfig.generated.h"

class UInputMappingContext;

/**
 * EGuestInputMode
 *
 * 입력 모드 열거형.
 * Stack 이 Top 일 때 이 값에 따라 PlayerController 입력 모드 전환.
 */
UENUM(BlueprintType)
enum class EGuestInputMode : uint8
{
    GameOnly    UMETA(DisplayName = "Game Only"),
    UIOnly      UMETA(DisplayName = "UI Only"),
    GameAndUI   UMETA(DisplayName = "Game And UI"),
};

/**
 * FGuestUIInputConfig (이름 충돌 방지를 위해 변경됨)
 *
 * Stack 하나에 대응하는 입력 설정 묶음.
 */
USTRUCT(BlueprintType)
struct GUEST_API FGuestUIInputConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGuestInputMode InputMode = EGuestInputMode::GameOnly;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputMappingContext> MappingContext = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MappingPriority = 0;
};