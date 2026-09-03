// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GNarrationTrigger.generated.h"

class UGNarrationDataAsset;

// 레벨에 도착하면 나레이션을 재생하는 액터.
// 퀘스트 완료가 아니라 "그 장소에 갔다"가 계기인 연출에 쓴다.
//
// 엔딩이 그 경우다. Stage 9에는 퀘스트가 없고, 중앙 도시에 도착한 것 자체가 조건이다.
UCLASS()
class GUEST_API AGNarrationTrigger : public AActor
{
	GENERATED_BODY()

public:
	AGNarrationTrigger();

	// 재생할 나레이션. 비어 있으면 아무 일도 하지 않는다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narration")
	TSoftObjectPtr<UGNarrationDataAsset> NarrationAsset;

	// 이 진행도 이상일 때만 재생한다. 0이면 진행도와 무관하게 재생.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narration")
	int32 RequiredStoryProgress = 0;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root;
};
