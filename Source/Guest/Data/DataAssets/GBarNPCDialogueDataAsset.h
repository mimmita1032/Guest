// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Guest/UI/Types/GuestUITypes.h"
#include "GBarNPCDialogueDataAsset.generated.h"

/**
 * 바 모드 손님 NPC 대화 데이터 에셋.
 * GuestUISubsystem::OpenBarDialogue()에 FBarDialogueData로 변환해 전달.
 */
UCLASS(BlueprintType)
class GUEST_API UGBarNPCDialogueDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 순서대로 재생할 대사 라인. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bar|Dialogue")
    FNPCDialogueData DialogueData;

    UFUNCTION(BlueprintPure, Category = "Bar|Dialogue")
    FBarDialogueData ToBarDialogueData() const
    {
        FBarDialogueData Out;
        Out.DialogueData = DialogueData;
        return Out;
    }
};
