// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GuestUITypes.generated.h"

/**
 * GuestUITypes.h
 *
 * UI 프레임워크 전반에서 사용하는 열거형 / 구조체 정의.
 * 별도 파일로 분리하여 순환 참조 없이 어느 UI 클래스에서나 포함 가능.
 */

/** 확인 모달 버튼 결과. */
UENUM(BlueprintType)
enum class EGuestConfirmResult : uint8
{
    Confirmed UMETA(DisplayName = "확인"),
    Cancelled UMETA(DisplayName = "취소"),
    Closed    UMETA(DisplayName = "닫힘"),
    Unknown   UMETA(DisplayName = "알 수 없음"),
};

/** 확인 모달에 전달할 설정 데이터. */
USTRUCT(BlueprintType)
struct FGuestConfirmData
{
    GENERATED_BODY()

    /** 모달 제목 텍스트. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText TitleText;

    /** 모달 본문 텍스트. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText BodyText;

    /** true = Yes/No 모드, false = OK/Cancel 모드. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bYesNoMode = false;
};
