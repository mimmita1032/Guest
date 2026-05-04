// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "UObject/Object.h"
#include "GItemInstance.generated.h"

class UGItemDefinition;
class UGItemFragment;


//인벤토리 핸들 확장성 안정성 고려되면 훗날 uint32 -> 언리얼 guid
USTRUCT()
struct FInventoryItemHandle
{
	GENERATED_BODY()
public:
	FInventoryItemHandle();
	static FInventoryItemHandle InvalidHandle();
	static FInventoryItemHandle CreateHandele();
	
	bool IsValid() const;
	uint32 GetHandleId() const { return HandleId; }
private:
	explicit FInventoryItemHandle(uint32 Id);
	
	UPROPERTY()
	uint32 HandleId;
	
	static uint32 GenerateNextId();
	static uint32 GetInvalidId();
};

FORCEINLINE bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs)
{
	return Lhs.GetHandleId() == Rhs.GetHandleId();
}
FORCEINLINE uint32 GetTypeHash(const FInventoryItemHandle& Key)
{
	return Key.GetHandleId();
}

/*========================
인벤토리에 실재하는 아이템 객체
========================*/
UCLASS(BlueprintType)
class GUEST_API UGItemInstance : public UObject
{
	GENERATED_BODY()
public:
	void InitInstance(const FInventoryItemHandle& NewHandle, TObjectPtr<const UGItemDefinition> InDef)
	{
		ItemDef = InDef;
		Handle = NewHandle;
	}
	
	template<typename T>
	const T* FindFragmentByClass() const;
	
	const UGItemDefinition* GetItemDef() const { return ItemDef; }
	FInventoryItemHandle GetHandle() const { return Handle; }
private:
	UPROPERTY()
	TObjectPtr<const UGItemDefinition> ItemDef;
	FInventoryItemHandle Handle;
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
