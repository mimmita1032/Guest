// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GuardDogAIController.generated.h"

/**
 * Controller dedicated to hostile guard dogs.
 * Perception and behavior-tree setup will be added in the following commits.
 */
UCLASS()
class GUEST_API AGuardDogAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGuardDogAIController();
};
