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
	// 개체별 아이콘 (사진의 촬영 스냅샷 등). 있으면 Icon보다 우선한다
	UTexture2D* RuntimeIcon = nullptr;
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

// 장비 부위 열거형
UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
	None,
	Helmet,
	Chest,
	Legs,
	Boots
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

	// ─── 기본 인벤토리(Grid) API ───
	
	// 아이템 정의로부터 인스턴스 생성 후 인벤토리에 자동 배치
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInventoryItemHandle GrantItem(const UGItemDefinition* ItemDef);

	// 개체별 데이터를 함께 실어 지급 (사진처럼 개체마다 내용이 다른 아이템용)
	// InstanceData는 FGItemInstanceData 파생 구조체여야 하며, 아니면 데이터 없이 지급된다
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInventoryItemHandle GrantItemWithData(const UGItemDefinition* ItemDef, const FInstancedStruct& InstanceData);

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

	// 어디든 놓을 자리가 있는지 확인 — 무거운 작업 전에 미리 거르는 용도
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	bool HasSpaceForItem(FIntPoint ItemSize) const;

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

	// ItemID로 인벤토리에서 첫 번째 매칭 핸들 검색 (없으면 InvalidHandle)
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FInventoryItemHandle FindHandleByItemID(FName ItemID) const;

	// 핸들로 아이템 크기 조회 (GridEntries 기반)
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	FIntPoint GetItemSize(FInventoryItemHandle Handle) const;

	// UI 렌더링 데이터 일괄 반환 — C++ 전용
	FInventoryItemRenderData GetItemRenderData(FInventoryItemHandle Handle) const;

	// ─── 장비 및 퀵슬롯 API ───

	// 아이템 장착 (그리드에서 제거하고 장비 슬롯으로 이동)
	UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
	bool EquipItem(FInventoryItemHandle Handle, EEquipSlot Slot);

	// 장착 해제 (장비 슬롯에서 제거하고 그리드의 특정 좌표 또는 빈 공간에 자동 배치)
	UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
	bool UnequipItem(EEquipSlot Slot, int32 TargetX = -1, int32 TargetY = -1);

	// 퀵슬롯 등록 (그리드 이동 없이 바로가기 링크만 할당)
	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickSlot")
	bool AssignQuickSlot(FInventoryItemHandle Handle, int32 SlotIndex);

	// 퀵슬롯 해제
	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickSlot")
	void ClearQuickSlot(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
	FInventoryItemHandle GetEquippedItem(EEquipSlot Slot) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|QuickSlot")
	FInventoryItemHandle GetQuickSlotItem(int32 SlotIndex) const;

	// ─── 저장/불러오기 API ───

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryChanged OnInventoryChanged;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Save")
	void ExportInventorySaveData(TArray<FGuestSavedInventoryEntry>& OutEntries) const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Save")
	void ImportInventorySaveData(const TArray<FGuestSavedInventoryEntry>& InEntries);
	
private:
	// ─── 인벤토리 상태 데이터 ───

	// 점유된 셀만 기록 (빈 셀은 항목 없음)
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory State")
	TMap<FIntPoint, FInventoryItemHandle> OccupiedSlots;

	// 핸들 → 인스턴스 (장착된 아이템도 여기에 포함됨)
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory State")
	TMap<FInventoryItemHandle, TObjectPtr<UGItemInstance>> InventoryMap;

	// 핸들 → 그리드 배치 정보 (좌상단 + 크기)
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory State")
	TMap<FInventoryItemHandle, FInventoryGridEntry> GridEntries;

	// 장착된 아이템 관리 (해당 아이템은 InventoryMap에는 있지만 Grid에는 없음)
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory State")
	TMap<EEquipSlot, FInventoryItemHandle> EquipmentSlots;

	// 퀵슬롯 (0~3 인덱스가 1~4번 슬롯을 의미. 아이템 자체는 Grid에 존재하며 참조만 가짐)
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory State")
	TArray<FInventoryItemHandle> QuickSlots;

	// ─── 비공개 헬퍼 ───

	bool CanPlaceAt(FIntPoint ItemSize, int32 StartX, int32 StartY, FInventoryItemHandle ExcludeHandle) const;
	void OccupySlots(FInventoryItemHandle Handle, FIntPoint ItemSize, int32 StartX, int32 StartY);
	void ClearSlotsForHandle(FInventoryItemHandle Handle);
	bool AutoPlace(FInventoryItemHandle Handle, FIntPoint ItemSize);

	void NotifyInventoryChanged();
	
	void ClearInventory();

	// 지정 좌표에 인스턴스를 만들어 배치 (세이브 복원용). InstanceData가 유효하면 함께 주입한다
	bool PlaceItemAt(const UGItemDefinition* ItemDef, FIntPoint TopLeft, FIntPoint Size,
		const FInstancedStruct& InstanceData = FInstancedStruct());
};