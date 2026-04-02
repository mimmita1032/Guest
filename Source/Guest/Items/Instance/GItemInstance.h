// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "UObject/Object.h"
#include "GItemInstance.generated.h"

class UGItemDefinition;
class UGItemFragment;

/*========================
인벤토리에 실재하는 아이템 객체
========================*/
UCLASS(BlueprintType)
class GUEST_API UGItemInstance : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	TObjectPtr<const UGItemDefinition> ItemDef;
	
	void SetItemDefinition(const TObjectPtr<const UGItemDefinition> InDef){ItemDef = InDef;}
	
	template<typename T>
	const T* FindFragmentByClass() const;
};

template <typename T>
const T* UGItemInstance::FindFragmentByClass() const
{
	if (ItemDef)
	{
		return ItemDef->FindFragmentByClass<T>(); 
	}
	return nullptr;
}
