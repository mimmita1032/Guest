// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Guest/Data/DataTable/GDialogueTypes.h"
#include "GDialogueDataAsset.generated.h"

class UDataTable;

/**
 * NPC별 대화 데이터 에셋.
 * 바 손님 NPC(ABarCustomerNPC)와 일반 월드 NPC(AGuestNPCBase) 공통 사용.
 *
 * 에디터에서 생성 후 NPC의 DialogueAsset 슬롯에 할당.
 * DialogueTable에 FDialogueNode 타입 DataTable을 연결하고 StartNodeID를 지정.
 */
UCLASS(BlueprintType)
class GUEST_API UGDialogueDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** 대화 시작 노드 ID (DataTable Row Name). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FName StartNodeID;

	/** FDialogueNode 타입 DataTable. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<UDataTable> DialogueTable;

	/** NodeID에 해당하는 노드를 반환한다. 없으면 nullptr. */
	const FDialogueNode* FindNode(FName NodeID) const;
};
