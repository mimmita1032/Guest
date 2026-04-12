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
	virtual void Interact(AActor* Interactor) = 0;

	// 출력용 텍스트 (UI 표시용)
	virtual FText GetInteractText() const { return FText::GetEmpty(); }
};