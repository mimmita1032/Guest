// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "GInventoryComponent.generated.h"

class UGItemDefinition;
class AGItemPickup;
class UTexture2D;

struct FGuestSavedInventoryEntry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

// UI 렌더링에 필요한 데이터를 한 번에 반환 (C++ 전용, USTRUCT 아님)
struct FInventoryItemRenderData
{
	TSoftObjectPtr<UTexture2D> Icon;
	FIntPoint GridSize = FIntPoint(1, 1);  // GridEntries 기준 (회전 대응)
	FIntPoint Position = FIntPoint(-1, -1); // 좌상단 그리드 좌표
	bool bIsValid      = false;
};

// 핸들별 그리드 배치 정보 (좌상단 좌표 + 크기)
USTRUCT()
struct FInventoryGridEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FIntPoint TopLeft = FIntPoint(0, 0);

	UPROPERTY()
	FIntPoint Size = FIntPoint(1, 1);
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUEST_API UGInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Settings")
	int32 Columns = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Settings")
	int32 Rows = 5;

	// 드롭 시 스폰할 픽업 액터 클래스 (미설정 시 AGItemPickup 기본 클래스 사용)
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Drop")
	TSubclassOf<AGItemPickup> DropPickupClass;

	// 아이템 정의로부터 인스턴스 생성 후 인벤토리에 자동 배치
	// 성공 시 발급된 핸들 반환, 실패(공간 없음) 시 InvalidHandle 반환
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInventoryItemHandle GrantItem(const UGItemDefinition* ItemDef);

	// 핸들 → 인스턴스
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UGItemInstance* GetItemByHandle(FInventoryItemHandle Handle) const;

	// 셀 좌표 → 핸들 (빈 셀이면 InvalidHandle 반환)
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	FInventoryItemHandle GetHandleAt(int32 X, int32 Y) const;

	// 셀 좌표 → 인스턴스 (빈 셀이면 nullptr)
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	UGItemInstance* GetItemAt(int32 X, int32 Y) const;

	// 아이템의 현재 좌상단 그리드 좌표 조회 (없으면 (-1,-1))
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	FIntPoint GetItemPosition(FInventoryItemHandle Handle) const;

	// 이동 가능 여부 확인 — 자기 셀은 자동 제외 (UI 드래그 하이라이트용)
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	bool CanMoveItemTo(FInventoryItemHandle Handle, int32 TargetX, int32 TargetY) const;

	// 특정 크기의 신규 아이템을 해당 좌표에 놓을 수 있는지 확인 (UI 프리뷰용)
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	bool CanPlaceNewItemAt(FIntPoint ItemSize, int32 StartX, int32 StartY) const;

	// 핸들로 아이템 이동
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	bool MoveItem(FInventoryItemHandle Handle, int32 TargetX, int32 TargetY);

	// 핸들로 아이템 제거 (인벤토리에서만 삭제)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FInventoryItemHandle Handle);

	// 핸들로 아이템 드롭 (인벤토리 제거 + 월드 픽업 스폰)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool DropItem(FInventoryItemHandle Handle);

	// 전체 핸들 목록 반환 (UI Refresh용)
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FInventoryItemHandle> GetAllHandles() const;

	// 핸들로 아이템 크기 조회 (GridEntries 기반 — 회전 적용 시 Fragment 크기와 다를 수 있음)
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	FIntPoint GetItemSize(FInventoryItemHandle Handle) const;

	// UI 렌더링 데이터 일괄 반환 — C++ 전용
	// GridEntry 또는 InventoryMap에서 Handle을 찾지 못하면 bIsValid=false 반환
	FInventoryItemRenderData GetItemRenderData(FInventoryItemHandle Handle) const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryChanged OnInventoryChanged;

	//save 용
	UFUNCTION(BlueprintCallable, Category = "Inventory|Save")
	void ExportInventorySaveData(TArray<FGuestSavedInventoryEntry>& OutEntries) const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Save")
	void ImportInventorySaveData(TArray<FGuestSavedInventoryEntry>& InEntries);
	
private:
	// 점유된 셀만 기록 (빈 셀은 항목 없음)
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory State")
	TMap<FIntPoint, FInventoryItemHandle> OccupiedSlots;

	// 핸들 → 인스턴스
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory State")
	TMap<FInventoryItemHandle, TObjectPtr<UGItemInstance>> InventoryMap;

	// 핸들 → 그리드 배치 정보 (좌상단 + 크기)
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory State")
	TMap<FInventoryItemHandle, FInventoryGridEntry> GridEntries;

	// 배치 가능 여부 핵심 로직
	// ExcludeHandle 셀은 점유되어 있어도 통과 (이동 시 자기 셀 제외용)
	bool CanPlaceAt(FIntPoint ItemSize, int32 StartX, int32 StartY, FInventoryItemHandle ExcludeHandle) const;

	// OccupiedSlots + GridEntries에 핸들 기록
	void OccupySlots(FInventoryItemHandle Handle, FIntPoint ItemSize, int32 StartX, int32 StartY);

	// OccupiedSlots + GridEntries에서 핸들 제거
	void ClearSlotsForHandle(FInventoryItemHandle Handle);

	// 첫 번째 가능한 위치에 자동 배치
	bool AutoPlace(FInventoryItemHandle Handle, FIntPoint ItemSize);

	void NotifyInventoryChanged();
	
	void ClearInventory();
	bool PlaceItemAt(const UGItemDefinition* ItemDef, FIntPoint TopLeft, FIntPoint Size);
};