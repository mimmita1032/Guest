// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GDialogueTypes.generated.h"

/** 대화 선택지 하나. */
USTRUCT(BlueprintType)
struct FDialogueChoice
{
	GENERATED_BODY()

	/** 플레이어에게 보여지는 선택지 텍스트. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString ChoiceText;

	/** 이 선택지 선택 시 이동할 노드 ID. NAME_None = 대화 종료. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FName NextNodeID;

	/** 이 선택지 표시 조건 ID (퀘스트 플래그 등). 비어있으면 항상 표시. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FName ConditionID;
};

/**
 * 대화 노드 하나. DataTable Row Name = NodeID.
 *
 * Choices가 비어있으면: Next 버튼 표시, NextNodeID로 자동 진행.
 * Choices가 있으면: 선택지 버튼 표시, NextNodeID 무시.
 * NextNodeID == NAME_None: 대화 종료.
 */
USTRUCT(BlueprintType)
struct FDialogueNode : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString DialogueText;

	/** 선택지 없을 때 다음 노드 ID. NAME_None = 대화 종료. Choices가 있으면 무시됨. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FName NextNodeID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FDialogueChoice> Choices;

	/**
	 * Choices가 없을 때 Btn_Next에 표시할 플레이어 응답 텍스트.
	 * 비어있으면 버튼 텍스트를 변경하지 않는다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString PlayerResponseText;

	/** 이 노드 도달 시 퀘스트 서브시스템에 전달할 이벤트 ID. 비어있으면 없음. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|Quest")
	FName QuestEventID;

	/**
	 * 이 노드 도달 시 달성 처리할 Talk 목표 ID. 비어있으면 없음.
	 *
	 * NPC가 아니라 노드가 들고 있는다. 같은 NPC라도 트리거로 다가가면 바 대화,
	 * E를 누르면 NPC 대화로 열리는데, 목표를 NPC에 두면 한쪽 경로에서만 발화한다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|Quest")
	FName TalkObjectiveID;
};
