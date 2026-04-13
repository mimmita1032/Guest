// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GuestAIController.generated.h"

UCLASS()
class GUEST_API AGuestAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGuestAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<class UBehaviorTree> BTAsset;
};