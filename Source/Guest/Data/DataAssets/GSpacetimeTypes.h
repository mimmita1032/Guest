// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GSpacetimeTypes.generated.h"

// 디카 작동 상태
UENUM(BlueprintType)
enum class EDigicamState : uint8
{
	Inactive,      // 비활성
	TimeSetting,   // 연도 설정 중 (시간 설정)
	LocationFocus, // 구역 포커싱 중 (장소 설정)
	ReadyToSnap    // 수거 준비 완료 (셔터 대기)
};

// 수거지(저기) 좌표 데이터 구조체
USTRUCT(BlueprintType)
struct FSpacetimeData : public FTableRowBase
{
	GENERATED_BODY()

	// 수거 대상 연도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacetime")
	int32 TargetYear = 2010;

	// 구역 고유 코드 (Area Code)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacetime")
	int32 AreaCode = 0;

	// 수거지 명칭
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacetime")
	FText PlaceName;

	// 목적지 레벨 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacetime")
	FName LevelName;

	// LCD 표시용 미리보기 텍스처 (저기의 풍경)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacetime")
	TSoftObjectPtr<UTexture2D> PreviewTexture;
};