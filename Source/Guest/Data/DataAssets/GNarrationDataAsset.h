// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GNarrationDataAsset.generated.h"

class UTexture2D;

// 나레이션 한 장면 (정지 일러스트 1장 + 텍스트 1개)
USTRUCT(BlueprintType)
struct FNarrationBeat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narration")
	TSoftObjectPtr<UTexture2D> Illustration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narration", meta = (MultiLine = true))
	FText Text;

	// 0보다 크면 이 시간(초) 후 입력 없이 자동으로 다음 장면 진행. 0이면 입력(Btn_Advance)으로만 진행.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narration")
	float AutoAdvanceDelay = 0.f;
};

/**
 * 퀘스트 완료 등 스토리 분기 전환 시 재생하는 정지 일러스트 나레이션 연출 데이터.
 * GuestNarrationWidgetBase가 Beats를 순서대로 재생 (일러스트 페이드 전환 + 텍스트 표시).
 */
UCLASS(BlueprintType)
class GUEST_API UGNarrationDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narration")
	TArray<FNarrationBeat> Beats;
};
