// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Guest/UI/Types/GuestUITypes.h"
#include "GNPCDialogueDataAsset.generated.h"

/**
 * NPC 대화 데이터 에셋.
 * 에디터에서 NPC별로 생성하고 AGuestNPCBase::DialogueDataAsset 슬롯에 할당한다.
 */
UCLASS(BlueprintType)
class GUEST_API UGNPCDialogueDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FNPCDialogueData DialogueData;
};
