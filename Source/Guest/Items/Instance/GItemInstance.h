// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Instance/GItemInstanceData.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "GItemInstance.generated.h"

class UGItemDefinition;
class UGItemFragment;


//인벤토리 핸들 확장성 안정성 고려되면 훗날 uint32 -> 언리얼 guid
USTRUCT(BlueprintType)
struct FInventoryItemHandle
{
	GENERATED_BODY()
public:
	FInventoryItemHandle();
	static FInventoryItemHandle InvalidHandle();
	static FInventoryItemHandle CreateHandle();
	
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

	/*---- 개체별 데이터 (설계도와 달리 개체마다 값이 다른 것) ----*/

	// 개체별 데이터 주입. FGItemInstanceData 파생 구조체를 담은 FInstancedStruct만 유효하다.
	// 인스턴스 생성 직후 1회 호출하는 것을 전제로 한다.
	void SetInstanceData(const FInstancedStruct& InInstanceData);

	// 타입 지정 조회 — 담긴 것이 T(또는 그 파생)가 아니면 nullptr
	template<typename T>
	const T* GetInstanceData() const;

	template<typename T>
	T* GetMutableInstanceData();

	// 세이브 직렬화 등 타입을 모른 채 통째로 다룰 때
	const FInstancedStruct& GetInstanceDataStruct() const { return InstanceData; }

	bool HasInstanceData() const { return InstanceData.IsValid(); }

private:
	UPROPERTY()
	TObjectPtr<const UGItemDefinition> ItemDef;
	FInventoryItemHandle Handle;

	// 개체 고유 상태. 비어 있으면 설계도만으로 완전히 설명되는 아이템이다.
	UPROPERTY()
	FInstancedStruct InstanceData;
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

template <typename T>
const T* UGItemInstance::GetInstanceData() const
{
	static_assert(TIsDerivedFrom<T, FGItemInstanceData>::IsDerived,
		"개체별 데이터는 FGItemInstanceData를 상속해야 합니다.");
	return InstanceData.GetPtr<T>();
}

template <typename T>
T* UGItemInstance::GetMutableInstanceData()
{
	static_assert(TIsDerivedFrom<T, FGItemInstanceData>::IsDerived,
		"개체별 데이터는 FGItemInstanceData를 상속해야 합니다.");
	return InstanceData.GetMutablePtr<T>();
}
