// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GuestGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GUEST_API AGuestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	AGuestGameModeBase();

	virtual void Tick(float DeltaTime) override;

protected:
	//플레이어 스폰 위치를 찾을 때 호출
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
};
