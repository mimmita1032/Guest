// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GNPCScheduleDataAsset.generated.h"

// NPC 시간대별 이동 스케줄 항목
USTRUCT(BlueprintType)
struct FNPCScheduleEntry
{
	GENERATED_BODY()

	// 시작 시각 — 시 (0 ~ 23)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|Start",
		meta = (ClampMin = "0", ClampMax = "23", UIMin = "0", UIMax = "23", DisplayName = "Start Hour"))
	int32 StartHour = 19;

	// 시작 시각 — 분 (0 ~ 59)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|Start",
		meta = (ClampMin = "0", ClampMax = "59", UIMin = "0", UIMax = "59", DisplayName = "Start Minute"))
	int32 StartMinute = 0;

	// 종료 시각 — 시 (0 ~ 23)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|End",
		meta = (ClampMin = "0", ClampMax = "23", UIMin = "0", UIMax = "23", DisplayName = "End Hour"))
	int32 EndHour = 23;

	// 종료 시각 — 분 (0 ~ 59)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|End",
		meta = (ClampMin = "0", ClampMax = "59", UIMin = "0", UIMax = "59", DisplayName = "End Minute"))
	int32 EndMinute = 0;

	// 목적지 액터의 태그 — 레벨에 배치한 웨이포인트 액터에 이 태그를 붙여두면 됨
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule")
	FName DestinationTag;

	// 에디터용 메모 (빌드에는 포함되지 않음)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule")
	FString Description;

	// 시작/종료 시각을 float (0.0 ~ 24.0) 으로 변환
	float GetStartAsFloat() const { return StartHour + StartMinute / 60.f; }
	float GetEndAsFloat()   const { return EndHour   + EndMinute   / 60.f; }
};

/**
 * NPC의 시간대별 이동 스케줄을 정의하는 데이터 에셋
 *
 * 사용법:
 *   1. Content Browser에서 이 에셋을 생성하고 Schedule 배열을 채운다.
 *   2. 레벨에 빈 Actor를 배치한 뒤 Tags 배열에 DestinationTag와 동일한 이름을 입력한다.
 *   3. GuestNPCBase의 ScheduleDataAsset 슬롯에 에셋을 할당한다.
 *   4. BehaviorTree에서 BB_bHasSchedule 키로 분기 후 BB_ScheduledDestination으로 MoveTo 한다.
 */
UCLASS(BlueprintType)
class GUEST_API UGNPCScheduleDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 시간대별 이동 스케줄 목록 (겹치지 않도록 설계할 것)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Schedule")
	TArray<FNPCScheduleEntry> Schedule;

	// 스케줄이 없는 시간대에 기본 배회를 허용할지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Schedule")
	bool bPatrolWhenNoSchedule = true;

	// 현재 시각에 활성화된 스케줄 항목을 반환. 없으면 nullptr.
	const FNPCScheduleEntry* FindActiveEntry(float CurrentHour) const;
};
