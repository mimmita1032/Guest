// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GQuestTypes.generated.h"

UENUM(BlueprintType)
enum class EQuestType : uint8
{
	Main,
	Sub,
	Skill,
	Hidden
};

UENUM(BlueprintType)
enum class EQuestObjectiveType : uint8
{
	Talk,
	Collect,
	Reach
};

// 퀘스트가 활성화되는 시간대 조건
UENUM(BlueprintType)
enum class EQuestTimeState : uint8
{
	Any,	// 시간 무관
	Past,	// 과거에서만 수락 가능
	Present // 현재에서만 수락 가능
};

/*=================
설계도용 목표 구조체 (DataTable 저장, 읽기 전용)
=================*/
USTRUCT(BlueprintType)
struct FQuestObjectiveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objective")
	EQuestObjectiveType ObjectiveType = EQuestObjectiveType::Collect;

	// 대화 상대 NPC ID / 수집 아이템 ID / 진입 트리거 볼륨 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objective")
	FName TargetID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objective")
	int32 RequiredAmount = 1;
};

/*=================
퀘스트 설계도 (DataTable Row, 읽기 전용)
=================*/
USTRUCT(BlueprintType)
struct FQuestData : public FTableRowBase
{
	GENERATED_BODY()

	// 퀘스트 고유 ID (예: Q_Main_001)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
	FName QuestID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
	EQuestType QuestType = EQuestType::Sub;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
	FText QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
	FText Description;

	// 이 퀘스트가 발생 가능한 시간대
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Prerequisites")
	EQuestTimeState RequiredTimeState = EQuestTimeState::Any;

	// 선행 퀘스트 ID (없으면 NAME_None)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Prerequisites")
	FName RequiredQuestID = NAME_None;

	// 단계별 목표 배열 (순서대로 진행)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objectives")
	TArray<FQuestObjectiveData> Objectives;

	// 완료 보상 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Reward")
	FName RewardItemID = NAME_None;

	// 완료 후 자동 연결될 다음 퀘스트 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Reward")
	FName NextQuestID = NAME_None;
};

/*=================
런타임 진행 상태 (서브시스템이 보유, 세이브 데이터에 포함 예정)
=================*/
USTRUCT(BlueprintType)
struct FQuestRuntimeData
{
	GENERATED_BODY()

	// 현재 진행 중인 목표 인덱스
	UPROPERTY(BlueprintReadOnly, Category = "Quest|Runtime")
	int32 CurrentStep = 0;

	// 각 목표의 현재 달성 수량 (Objectives 배열과 인덱스 1:1 대응)
	UPROPERTY(BlueprintReadOnly, Category = "Quest|Runtime")
	TArray<int32> ObjectiveCounts;
};
