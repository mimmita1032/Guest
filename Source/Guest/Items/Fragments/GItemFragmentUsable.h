// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GItemFragment.h"
#include "GItemFragmentUsable.generated.h"

/*=================
사용용도 카테고리
=================*/
UENUM(BlueprintType)
enum class EUsableCategory : uint8
{
	None,
};

/*=================
아이템 사용
=================*/
UCLASS()
class GUEST_API UGItemFragmentUsable : public UGItemFragment
{
	GENERATED_BODY()
	
public:
	//사용시 발생할 효과 수치
	UPROPERTY(EditDefaultsOnly, Category = "Usable")
	float EffectValue = 0.0f;
};
