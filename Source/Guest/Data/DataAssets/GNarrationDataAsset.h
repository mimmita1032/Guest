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

	// 나레이션이 끝난 뒤 진행시킬 날짜 수 (0이면 진행 없음).
	// "꽃을 놓았다 → 나레이션 → 하루 뒤 스미스가 찾아온다" 같은 시간 경과 연출용.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narration|시간 경과")
	int32 DaysToAdvanceOnFinish = 0;

	// 날짜를 진행시킬 때 맞출 시각 (0~24). 음수면 시각은 건드리지 않는다.
	// 예: 9.0을 넣으면 "다음 날 아침 9시"가 된다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narration|시간 경과",
		meta = (EditCondition = "DaysToAdvanceOnFinish > 0"))
	float HourOnFinish = -1.0f;

	// 나레이션이 끝난 뒤 올릴 스토리 진행도 (0이면 변화 없음).
	// 퀘스트 완료 즉시 올리면 나레이션이 화면을 덮기도 전에 세계가 바뀐다 —
	// 게이팅된 NPC가 연출 도중에 나타나버린다. 연출이 끝난 뒤에 바뀌어야 할 것은 여기에 둔다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narration|시간 경과")
	int32 StoryProgressOnFinish = 0;

	/**
	 * 나레이션이 걷힌 뒤 데모 종료 화면을 띄울지 여부.
	 * 엔딩 나레이션에만 켠다. 켜면 화면이 닫히는 대신 종료 화면으로 넘어간다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narration|Finish")
	bool bShowDemoEndOnFinish = false;
};
