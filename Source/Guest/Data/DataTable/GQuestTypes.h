// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GQuestTypes.generated.h"

class UGNarrationDataAsset;

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
	Reach,
	Place,
	// 사진 촬영. TargetID는 FPhotoData::SubjectID(= 촬영 좌표의 PhotoSubjectID)와 대조된다
	Photo
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

	// 단계 내에서 이 목표를 식별하는 고유 ID (기획자가 DataTable에 직접 입력)
	// 진행도를 인덱스 대신 이 ID로 저장/복원 — 목표 순서 변경/삽입에도 세이브가 깨지지 않음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objective")
	FName ObjectiveID = NAME_None;

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

	// 퀘스트 내에서 이 단계를 식별하는 고유 ID (기획자가 DataTable에 직접 입력)
	// 진행 단계를 인덱스 대신 이 ID로 저장/복원 — 단계 순서 변경/삽입에도 세이브가 깨지지 않음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
	FName StepID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Step")
	TArray<FQuestObjectiveData> Objectives;

	// ID로 목표 조회 (없으면 nullptr)
	const FQuestObjectiveData* FindObjectiveByID(FName InObjectiveID) const
	{
		if (InObjectiveID.IsNone()) return nullptr;
		for (const FQuestObjectiveData& Obj : Objectives)
		{
			if (Obj.ObjectiveID == InObjectiveID) return &Obj;
		}
		return nullptr;
	}
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

	// 이 퀘스트를 수락 가능한 최소 스토리 진행도 (0 = 진행도 무관)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Prerequisites")
	int32 RequiredStoryProgress = 0;

	// 단계 배열 - 순서대로 진행, 각 단계 내 Objectives는 동시 달성
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Steps")
	TArray<FQuestStepData> Steps;

	// 완료 보상 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Reward")
	FName RewardItemID = NAME_None;

	// 완료 후 자동 연결될 다음 퀘스트 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Reward")
	FName NextQuestID = NAME_None;

	// 완료 시 스토리 진행도를 이 값으로 올림 (0 = 변화 없음, 현재 진행도보다 낮으면 무시)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Reward")
	int32 StoryProgressOnComplete = 0;

	// 완료 시 재생할 나레이션 연출 (일러스트+텍스트 슬라이드쇼). 비어있으면 재생하지 않음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Reward")
	TSoftObjectPtr<UGNarrationDataAsset> NarrationOnComplete;

	// StepID로 단계 인덱스 조회 (없으면 INDEX_NONE)
	int32 FindStepIndexByID(FName InStepID) const
	{
		if (InStepID.IsNone()) return INDEX_NONE;
		for (int32 Index = 0; Index < Steps.Num(); ++Index)
		{
			if (Steps[Index].StepID == InStepID) return Index;
		}
		return INDEX_NONE;
	}

	// StepID로 단계 조회 (없으면 nullptr)
	const FQuestStepData* FindStepByID(FName InStepID) const
	{
		const int32 Index = FindStepIndexByID(InStepID);
		return Steps.IsValidIndex(Index) ? &Steps[Index] : nullptr;
	}
};

/*=================
목표 하나의 진행 수량 (런타임/세이브 공용 — 인덱스 대신 ObjectiveID로 매칭)
=================*/
USTRUCT(BlueprintType)
struct FQuestObjectiveProgress
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Runtime")
	FName ObjectiveID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Runtime")
	int32 Count = 0;
};

/*=================
런타임 진행 상태 (서브시스템 보유, 세이브 데이터 포함)
=================*/
USTRUCT(BlueprintType)
struct FQuestRuntimeData
{
	GENERATED_BODY()

	// 현재 진행 중인 단계 ID
	UPROPERTY(BlueprintReadOnly, Category = "Quest|Runtime")
	FName CurrentStepID = NAME_None;

	// 현재 단계 내 각 목표의 달성 수량 (ObjectiveID로 매칭)
	UPROPERTY(BlueprintReadOnly, Category = "Quest|Runtime")
	TArray<FQuestObjectiveProgress> ObjectiveProgress;

	// 단계 진입 시 진행 장부를 해당 단계의 목표들로 초기화 (수량 0)
	void InitializeForStep(const FQuestStepData& Step)
	{
		CurrentStepID = Step.StepID;
		ObjectiveProgress.Reset();
		ObjectiveProgress.Reserve(Step.Objectives.Num());
		for (const FQuestObjectiveData& Obj : Step.Objectives)
		{
			FQuestObjectiveProgress Progress;
			Progress.ObjectiveID = Obj.ObjectiveID;
			ObjectiveProgress.Add(Progress);
		}
	}

	// ID로 진행 항목 조회 (없으면 nullptr)
	FQuestObjectiveProgress* FindObjectiveProgress(FName InObjectiveID)
	{
		for (FQuestObjectiveProgress& Progress : ObjectiveProgress)
		{
			if (Progress.ObjectiveID == InObjectiveID) return &Progress;
		}
		return nullptr;
	}

	// ID에 해당하는 목표의 달성 수량 (없으면 0)
	int32 GetObjectiveCount(FName InObjectiveID) const
	{
		for (const FQuestObjectiveProgress& Progress : ObjectiveProgress)
		{
			if (Progress.ObjectiveID == InObjectiveID) return Progress.Count;
		}
		return 0;
	}
};
