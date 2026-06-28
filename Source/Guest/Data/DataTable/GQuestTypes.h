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
	Any,
	Past,
	Present
};

/*=================
목표 하나 (TargetID 기준으로 진행 카운트)
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

	// UI에 표시할 목표 설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objective")
	FString ObjectiveText;
};

/*=================
단계 하나 (동시에 달성해야 하는 목표들의 묶음)
한 단계의 모든 Objectives가 완료되어야 다음 단계로 진행
=================*/
USTRUCT(BlueprintType)
struct FQuestStepData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
	TArray<FQuestObjectiveData> Objectives;
};

/*=================
퀘스트 설계도 (DataTable Row, 읽기 전용)
=================*/
USTRUCT(BlueprintType)
struct FQuestData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
	EQuestType QuestType = EQuestType::Sub;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
	FString QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
	FString Description;

	// 이 퀘스트가 발생 가능한 시간대
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Prerequisites")
	EQuestTimeState RequiredTimeState = EQuestTimeState::Any;

	// 선행 퀘스트 ID (없으면 NAME_None)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Prerequisites")
	FName RequiredQuestID = NAME_None;

	// 단계 배열 - 순서대로 진행, 각 단계 내 Objectives는 동시 달성
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Steps")
	TArray<FQuestStepData> Steps;

	// 완료 보상 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Reward")
	FName RewardItemID = NAME_None;

	// 완료 후 자동 연결될 다음 퀘스트 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Reward")
	FName NextQuestID = NAME_None;
};

/*=================
런타임 진행 상태 (서브시스템 보유, 세이브 데이터 포함)
=================*/
USTRUCT(BlueprintType)
struct FQuestRuntimeData
{
	GENERATED_BODY()

	// 현재 진행 중인 단계 인덱스
	UPROPERTY(BlueprintReadOnly, Category = "Quest|Runtime")
	int32 CurrentStep = 0;

	// 현재 단계 내 각 목표의 달성 수량 (Steps[CurrentStep].Objectives와 인덱스 1:1 대응)
	UPROPERTY(BlueprintReadOnly, Category = "Quest|Runtime")
	TArray<int32> ObjectiveCounts;
};
