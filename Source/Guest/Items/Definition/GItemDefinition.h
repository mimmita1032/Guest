// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GItemDefinition.generated.h"

/*=================
아이템 카테고리
=================*/
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	None,
};

class UGItemFragment;
/*=================
아이템 설계도
=================*/
UCLASS()
class GUEST_API UGItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	//아이템 이름
	UPROPERTY(EditDefaultsOnly, Category = "Default")
	FText ItemName;

	//아이템 코드
	UPROPERTY(EditDefaultsOnly, Category = "Default")
	FName ItemID;
	
	//아이템 카테고리
	UPROPERTY(EditDefaultsOnly, Category = "Default")
	EItemCategory Category;
	
	
	//Fragment 조립 배열
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Fragments")
	TArray<TObjectPtr<UGItemFragment>> Fragments;
	
	//Fragment 검색
	template<typename T>
	const T* FindFragmentByClass() const;
};
