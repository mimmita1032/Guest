// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/Environment/Props/GDoorBase.h"
#include "GDoorLevelTransition.generated.h"

UCLASS()
class GUEST_API AGDoorLevelTransition : public AGDoorBase
{
	GENERATED_BODY()

public:
	// 상호작용할 때 호출
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	// 이동할 실제 맵 파일의 이름 (예: L_TavernMain)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
	FName TargetLevelName;

	// 목적지 레벨에서 찾을 PlayerStart의 고유 태그 (예: Spawn_FrontDoor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
	FName TargetSpawnTag;
};
