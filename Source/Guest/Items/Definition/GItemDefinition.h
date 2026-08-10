// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GItemDefinition.generated.h"

/*=================
아이템 카테고리 (UI 필터링, 탭 정렬 등에 사용)
=================*/
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	None,
	Equipment   UMETA(DisplayName = "장비"),
	Consumable  UMETA(DisplayName = "소비품"),
	Material    UMETA(DisplayName = "재료"),
	Quest       UMETA(DisplayName = "퀘스트")
};

class UGItemFragment;

/*=================
아이템 설계도
=================*/
UCLASS(BlueprintType, Blueprintable)
class GUEST_API UGItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetGuestItemAssetType();
	
	// 아이템 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	FText ItemName;

	// 아이템 코드 (Save/Load 식별자)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	FName ItemID;
	
	// 아이템 카테고리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	EItemCategory Category;

	// ★ 아이템 고유 속성 태그들 (장비 부위 판별, 퀵슬롯 등록 가능 여부 등에 사용)
	// 예: Guest.Item.Equip.Helmet, Guest.Item.Consumable 등
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer ItemTags;
	
	// Fragment 조립 배열
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Fragments")
	TArray<TObjectPtr<UGItemFragment>> Fragments;
	
	// Fragment 검색 (C++ 전용)
	template<typename T>
	const T* FindFragmentByClass() const
	{
		for (const auto& Fragment : Fragments)
		{
			if (const T* FoundFragment = Cast<T>(Fragment)) return FoundFragment;
		}
		return nullptr;
	}

	// 특정 태그 포함 여부 확인 함수 (UI 드래그 앤 드롭 시 부위 판별에 사용)
	UFUNCTION(BlueprintPure, Category = "Tags")
	bool HasTag(FGameplayTag TagToCheck) const
	{
		return ItemTags.HasTag(TagToCheck);
	}

	// 여러 태그 중 하나라도 포함하는지 확인 (유연한 슬롯 조건 판별용)
	UFUNCTION(BlueprintPure, Category = "Tags")
	bool HasAnyMatchingTags(const FGameplayTagContainer& TagsToCheck) const
	{
		return ItemTags.HasAny(TagsToCheck);
	}
};