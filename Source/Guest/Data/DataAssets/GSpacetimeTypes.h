// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
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

	// 스토리상 날짜 (예: "1998년 3월 15일") — 기획 확정 전 임시값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacetime")
	FText StoryDate;
};

// 촬영된 사진 한 장의 데이터
USTRUCT(BlueprintType)
struct FPhotoData
{
	GENERATED_BODY()

	// 촬영된 이미지 스냅샷 (GameInstance outer — 레벨 전환 후에도 유지)
	UPROPERTY(BlueprintReadOnly, Category = "Photo")
	TObjectPtr<UTexture2D> Snapshot = nullptr;

	// 이동한 in-game 연도
	UPROPERTY(BlueprintReadOnly, Category = "Photo")
	int32 InGameYear = 0;

	// 장소명
	UPROPERTY(BlueprintReadOnly, Category = "Photo")
	FText PlaceName;

	// 구역 코드
	UPROPERTY(BlueprintReadOnly, Category = "Photo")
	int32 AreaCode = 0;

	// 스토리상 날짜 (DataTable StoryDate에서 복사)
	UPROPERTY(BlueprintReadOnly, Category = "Photo")
	FText StoryDate;

	// 현실 촬영 시각
	UPROPERTY(BlueprintReadOnly, Category = "Photo")
	FDateTime RealWorldTime;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhotoTaken, const FPhotoData&, Photo);