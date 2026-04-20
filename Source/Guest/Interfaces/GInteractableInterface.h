// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GInteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UGInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class GUEST_API IGInteractableInterface
{
	GENERATED_BODY()

public:
	// 상호작용 실행
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);

	// UI 표시용 텍스트 반환
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractText() const;
};