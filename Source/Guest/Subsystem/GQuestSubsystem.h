// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Guest/Data/DataTable/GQuestTypes.h"
#include "GQuestSubsystem.generated.h"


class UGuestSaveGame;
struct FGuestSavedActiveQuestEntry;
class UGNarrationDataAsset;

// 아이템 획득 / NPC 대화 / 구역 진입 시 Broadcast → 서브시스템이 수신하여 장부 갱신
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestObjectiveUpdated, FName, TargetID, int32, Amount);

// 퀘스트 수락/완료/목표 진행 시 Broadcast → UI 트래커가 구독하여 갱신
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestListChanged);

// 퀘스트 완료 시 NarrationOnComplete가 지정돼 있으면 Broadcast → UI가 구독해 나레이션 연출 재생
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNarrationRequested, UGNarrationDataAsset*, NarrationAsset);

UCLASS()
class GUEST_API UGQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

#pragma region Delegate
public:
	// 아이템/NPC/트리거가 이 델리게이트에 Broadcast하면 서브시스템이 자동으로 장부를 갱신
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestObjectiveUpdated OnObjectiveUpdated;

	// 퀘스트 상태 변경 시 Broadcast (수락/완료/목표 진행) → UI 트래커용
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestListChanged OnQuestListChanged;

	// 퀘스트 완료 + NarrationOnComplete 지정 시 Broadcast → UI가 구독해 나레이션 위젯 오픈
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnNarrationRequested OnNarrationRequested;
#pragma endregion

#pragma region Quest Flow
public:
	// NPC 대화 완료 시점 등에서 호출 → 조건 검증 후 진행 장부 등록
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void AcceptQuest(FName QuestID);

	// 활성 퀘스트 진행 중인지 확인
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool IsQuestActive(FName QuestID) const;

	// 퀘스트 완료 여부 확인
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool IsQuestCompleted(FName QuestID) const;

	// 진행 중인 퀘스트의 현재 단계 ID (활성이 아니면 NAME_None)
	UFUNCTION(BlueprintCallable, Category = "Quest")
	FName GetCurrentStepID(FName QuestID) const;

	// 현재 진행 중인 퀘스트 목록 반환
	UFUNCTION(BlueprintCallable, Category = "Quest")
	TArray<FName> GetActiveQuestIDs() const;

	// 완료된 퀘스트 목록 반환
	UFUNCTION(BlueprintCallable, Category = "Quest")
	TArray<FName> GetCompletedQuestIDs() const;

	// UI에서 퀘스트 진행 상태 조회
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool GetQuestRuntimeData(FName QuestID, FQuestRuntimeData& OutData) const;

	// UI에서 퀘스트 정적 데이터 조회
	const FQuestData* FindQuestDataPublic(FName QuestID) const { return FindQuestData(QuestID); }

	/**
	 * 디버그 전용 — 선행 퀘스트 사슬을 거슬러 올라가 전부 완료 처리한 뒤 목표 퀘스트를 수락한다.
	 *
	 * 뒤쪽 스테이지를 확인하려고 매번 처음부터 플레이하지 않기 위한 것이다.
	 * 진행도만 올려서는 안 된다 — AcceptQuest가 RequiredQuestID를 실제 완료 장부에서 확인한다.
	 */
	void DebugSkipToQuest(FName QuestID);

private:
	// OnObjectiveUpdated 델리게이트 수신 핸들러
	UFUNCTION()
	void HandleObjectiveUpdated(FName TargetID, int32 Amount);

	// 모든 목표 달성 시 내부 호출 → 보상 지급 + 다음 퀘스트 연결
	void CompleteQuest(FName QuestID);
#pragma endregion

#pragma region Story Progress
public:
	// 현재 스토리 진행도 조회 (RequiredStoryProgress 게이팅용)
	UFUNCTION(BlueprintCallable, Category = "Quest|Story")
	int32 GetStoryProgress() const { return StoryProgress; }

	// 스토리 진행도 갱신 (퀘스트 완료/이벤트 트리거 등에서 호출, 세이브 로드 시 복원에도 사용)
	UFUNCTION(BlueprintCallable, Category = "Quest|Story")
	void SetStoryProgress(int32 NewProgress);

private:
	UPROPERTY()
	int32 StoryProgress = 0;
#pragma endregion

#pragma region Data
protected:
	// 마스터 퀘스트 데이터 테이블
	UPROPERTY()
	TObjectPtr<UDataTable> QuestDataTable;

private:
	// 진행 중 장부: QuestID → 런타임 진행 상태
	TMap<FName, FQuestRuntimeData> ActiveQuests;

	// 완료 장부: TSet → O(1) Contains 검색
	TSet<FName> CompletedQuests;

	// 헬퍼: DataTable에서 퀘스트 데이터 조회
	const FQuestData* FindQuestData(FName QuestID) const;

	// 진행/저장 매칭 키인 StepID/ObjectiveID가 비었거나 중복인 행을 기동 시점에 경고 (기획 데이터 입력 유도)
	void ValidateQuestDataTableIDs() const;
#pragma endregion
	
	
#pragma region  save
	
public:
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	void ExportQuestSaveData(
		TArray<FGuestSavedActiveQuestEntry>& OutActiveQuests,
		TArray<FName>& OutCompletedQuestIDs) const;
	
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	void ImportQuestSaveData(
		const TArray<FGuestSavedActiveQuestEntry>& InActiveQuests,
		const TArray<FName>& InCompletedQuestIDs);

	
#pragma endregion
};
