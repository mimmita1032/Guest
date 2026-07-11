// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GQuestReachTrigger.generated.h"

// 레벨 진입만으로 Reach 타입 퀘스트 목표를 완료 처리해야 하는 경우(귀가 등)에 목적지 레벨에 배치.
// 디지캠 시공간 이동으로 이 레벨에 도착하면 BeginPlay에서 자동으로 목표를 갱신한다.
UCLASS()
class GUEST_API AGQuestReachTrigger : public AActor
{
	GENERATED_BODY()

public:
	AGQuestReachTrigger();

	// DT_QuestData의 Reach 목표 TargetID와 일치해야 함
	UPROPERTY(EditAnywhere, Category = "Quest")
	FName TargetID = NAME_None;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root;
};
