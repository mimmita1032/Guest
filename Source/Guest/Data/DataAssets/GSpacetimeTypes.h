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

// 시공간 검색 결과 — UI 피드백 분기용 (일치하는 좌표 없음 / 스토리 잠김 / 이동 가능)
UENUM(BlueprintType)
enum class ESpacetimeSearchResult : uint8
{
	NoMatch,
	Locked,
	Found
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

	// 이 좌표가 열리는 최소 스토리 진행도 (0 = 처음부터 열림)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacetime")
	int32 RequiredStoryProgress = 0;

	// 이 좌표에서 찍은 사진에 기록될 촬영 대상 식별자.
	// 사진 퀘스트 목표(EQuestObjectiveType::Photo)의 TargetID와 대조된다.
	// 비어 있으면 그 좌표의 사진은 어떤 퀘스트 목표도 진행시키지 않는다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacetime")
	FName PhotoSubjectID;
};

// 촬영된 사진 한 장의 데이터
USTRUCT(BlueprintType)
struct FPhotoData
{
	GENERATED_BODY()

	// 촬영된 이미지 스냅샷 (GameInstance outer — 레벨 전환 후에도 유지)
	// 런타임 전용 텍스처라 세이브에 직렬화되지 않음 — 로드 시 CompressedImage에서 재생성
	UPROPERTY(BlueprintReadOnly, Category = "Photo")
	TObjectPtr<UTexture2D> Snapshot = nullptr;

	// 세이브 보존용 PNG 압축 데이터 (촬영 시점에 1회 생성)
	UPROPERTY()
	TArray<uint8> CompressedImage;

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

	// 무엇을 찍었는가 (퀘스트 목표 판정용).
	// 지금은 촬영 좌표의 FSpacetimeData::PhotoSubjectID가 그대로 들어간다 — 장소 단위 판정.
	// 추후 카메라에 피사체 조준(트레이스)이 생기면 이 필드를 실제 피사체로 채우면 되고,
	// 퀘스트 데이터와 판정 로직은 그대로 둘 수 있다.
	UPROPERTY(BlueprintReadOnly, Category = "Photo")
	FName SubjectID;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhotoTaken, const FPhotoData&, Photo);