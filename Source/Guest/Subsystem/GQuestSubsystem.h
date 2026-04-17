// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Guest/Data/DataTable/GQuestTypes.h"
#include "GQuestSubsystem.generated.h"

// 아이템 획득 / NPC 대화 / 구역 진입 시 Broadcast → 서브시스템이 수신하여 장부 갱신
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestObjectiveUpdated, FName, TargetID, int32, Amount);

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

	// 현재 진행 중인 퀘스트 목록 반환
	UFUNCTION(BlueprintCallable, Category = "Quest")
	TArray<FName> GetActiveQuestIDs() const;

	// 완료된 퀘스트 목록 반환
	UFUNCTION(BlueprintCallable, Category = "Quest")
	TArray<FName> GetCompletedQuestIDs() const;

private:
	// OnObjectiveUpdated 델리게이트 수신 핸들러
	UFUNCTION()
	void HandleObjectiveUpdated(FName TargetID, int32 Amount);

	// 모든 목표 달성 시 내부 호출 → 보상 지급 + 다음 퀘스트 연결
	void CompleteQuest(FName QuestID);
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
#pragma endregion
};
