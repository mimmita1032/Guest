// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GItemData.generated.h"

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	None,
	Collectible UMETA(DisplayName = "Collectibles"),
	Usable		UMETA(DisplayName = "Expendables")
};

UCLASS(BlueprintType)
class GUEST_API UGItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 아이템 고유 ID (데이터 테이블 연동 시 사용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemInfo")
	FName ItemID;

	// UI 및 화면에 표시될 아이템 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemInfo")
	FText ItemName;

	// 아이템이 속한 현실 세계의 연도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narrative")
	int32 OriginYear;

	// 플레이어가 아이템을 관찰할 때 읽을 수 있는 사연/텍스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narrative", meta = (MultiLine = true))
	FText Description;

	// 에디터에서 보여질 아이템의 3D 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<class UStaticMesh> ItemMesh;
	
	// 인벤토리에서 보여질 아이템 아이콘 -> 회의 필요
	/*
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Visuals")
	TObjectPtr<UTexture2D> ItemIcon;
	*/
	
	// 아이템 Type (어떤 용도로 사용되는 것인지 구분)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Info")
    EItemCategory Category;
	
	// Slot Occupancy를 위한 Size
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Inventory")
	FIntPoint GridSize = FIntPoint(1, 1);
};