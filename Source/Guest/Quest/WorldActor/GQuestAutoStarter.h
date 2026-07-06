// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GQuestAutoStarter.generated.h"

// NPC 대화 없이 레벨 진입만으로 퀘스트를 시작해야 하는 경우(오프닝 등)에 레벨에 배치
UCLASS()
class GUEST_API AGQuestAutoStarter : public AActor
{
	GENERATED_BODY()

public:
	AGQuestAutoStarter();

	// 레벨 진입 시 자동으로 수락시킬 퀘스트 ID
	UPROPERTY(EditAnywhere, Category = "Quest")
	FName QuestID = NAME_None;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root;
};
