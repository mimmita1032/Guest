// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GItemFragment.generated.h"


/*=================
아이템 카테고리
=================*/
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	None,
};

/*=================
모든 Fragment의 조상
=================*/
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class GUEST_API UGItemFragment : public UObject
{
	GENERATED_BODY()
	
public:
	//아이템 이름
	UPROPERTY(EditDefaultsOnly, Category = "Narrative")
	FText ItemName;

	//아이템 코드
	UPROPERTY(EditDefaultsOnly, Category = "Narrative")
	FName ItemID;
	
	//아이템 카테고리
	UPROPERTY(EditDefaultsOnly, Category = "Narrative")
	EItemCategory Category;
};
