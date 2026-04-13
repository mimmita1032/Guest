// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "GuestUISettings.generated.h"

/** Tag → 위젯 클래스 단일 매핑 항목. */
USTRUCT()
struct FGuestWidgetMapping
{
    GENERATED_BODY()

    /** 위젯 식별 태그. 예: Guest.Widget.CameraUI. */
    UPROPERTY(EditAnywhere, config)
    FGameplayTag WidgetTag;

    /**
     * 연결할 위젯 클래스.
     * 소프트 레퍼런스로 선언하여 게임 시작 시 전부 로드되지 않고
     * PushWidgetByTag 호출 시점에 비동기로 로드됨.
     */
    UPROPERTY(EditAnywhere, config)
    TSoftClassPtr<UUserWidget> WidgetClass;
};


/**
 * UGuestUISettings
 *
 * UI 프레임워크 전역 설정 클래스.
 * Tag → 위젯 클래스 매핑 테이블을 한 곳에서 관리.
 * 코드 수정 없이 DefaultGame.ini 또는
 * Project Settings > Game > Guest UI Settings 에서 편집 가능.
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Guest UI Settings"))
class GUEST_API UGuestUISettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    UGuestUISettings();

    /**
     * Primary Layout 위젯 클래스.
     * 모든 Widget Stack 을 포함하는 최상위 레이아웃.
     * GuestPlayerController::BeginPlay 에서 동기 로드 후 뷰포트에 추가됨.
     */
    UPROPERTY(config, EditAnywhere, Category = "Widgets")
    TSoftClassPtr<UUserWidget> PrimaryLayoutClass;

    /** Tag → 위젯 클래스 매핑 배열. PushWidgetByTag 에서 조회됨. */
    UPROPERTY(config, EditAnywhere, Category = "Widgets")
    TArray<FGuestWidgetMapping> WidgetMappings;

    /**
     * WidgetTag 에 대응하는 위젯 소프트 클래스 반환.
     * 없으면 nullptr 반환 및 Warning 로그 출력.
     */
    TSoftClassPtr<UUserWidget> FindWidgetClassByTag(const FGameplayTag& Tag) const;
};
