// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GuestBlueprintLibrary.generated.h"

class UGuestUISubsystem;
class UTextLayoutWidget;

UCLASS()
class GUEST_API UGuestBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Guest|UI", meta = (WorldContext = "WorldContextObject", CompactNodeTitle = "GuestUI"))
    static UGuestUISubsystem* GetGuestUISubsystem(const UObject* WorldContextObject);

    /**
     * 가변 길이 텍스트가 화면 밖으로 잘리지 않도록 자동 줄바꿈을 켠다.
     * TextBlock / RichTextBlock / CommonTextBlock 모두 UTextLayoutWidget 파생이라 함께 받는다.
     *
     * WrapTextAt <= 0 이면 부모가 준 폭에서 접는다. 부모가 폭을 잡아주지 않는 배치
     * (CanvasPanel 자동 크기, Auto 사이즈 HorizontalBox 슬롯 등)에서는 자동 줄바꿈만으로는
     * 안 접히므로 그럴 때만 픽셀 폭을 직접 준다.
     */
    UFUNCTION(BlueprintCallable, Category = "Guest|UI")
    static void ApplyAutoWrap(UTextLayoutWidget* TextWidget, float WrapTextAt = 0.f);
};
