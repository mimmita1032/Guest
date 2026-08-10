// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventoryComponent.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "Guest/Items/WorldActor/GItemPickup.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Save/GuestSaveGame.h"
#include "Guest/FrameWork/GuestAssetManager.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"

namespace
{
	const UGItemDefinition* FindItemDefinitionByItemID(FName ItemID)
	{
		if (ItemID.IsNone())
		{
			return nullptr;
		}

		if (GEngine && GEngine->AssetManager)
		{
			if (const UGuestAssetManager* GuestAM = Cast<UGuestAssetManager>(GEngine->AssetManager.Get()))
			{
				TArray<const UGItemDefinition*> LoadedItems;
				if (GuestAM->GetLoadedItems(LoadedItems))
				{
					for (const UGItemDefinition* Def : LoadedItems)
					{
						if (Def && Def->ItemID == ItemID)
						{
							return Def;
						}
					}
				}
			}
		}

		const FAssetRegistryModule& AssetRegistryModule =
			FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetDatas;
		AssetRegistryModule.Get().GetAssetsByClass(
			UGItemDefinition::StaticClass()->GetClassPathName(), AssetDatas, true);

		for (const FAssetData& AssetData : AssetDatas)
		{
			const UGItemDefinition* Def = Cast<UGItemDefinition>(AssetData.GetAsset());
			if (Def && Def->ItemID == ItemID)
			{
				return Def;
			}
		}

		return nullptr;
	}
}

UGInventoryComponent::UGInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// 퀵슬롯 4칸 초기화 (Invalid Handle로 채움)
	QuickSlots.Init(FInventoryItemHandle(), 4);

	UE_LOG(LogGSystem, Log, TEXT("인벤토리 초기화: %d x %d (%d 슬롯)"), Columns, Rows, Columns * Rows);
}

// ─── 공개 API (기본 인벤토리) ──────────────────────────────────────────────────

FInventoryItemHandle UGInventoryComponent::GrantItem(const UGItemDefinition* ItemDef)
{
	return GrantItemWithData(ItemDef, FInstancedStruct());
}

FInventoryItemHandle UGInventoryComponent::GrantItemWithData(const UGItemDefinition* ItemDef, const FInstancedStruct& InstanceData)
{
	if (!ItemDef) return FInventoryItemHandle();

	const FInventoryItemHandle Handle = FInventoryItemHandle::CreateHandle();
	UGItemInstance* Instance = NewObject<UGItemInstance>(this);
	Instance->InitInstance(Handle, ItemDef);

	// 빈 구조체면 내부에서 "데이터 없음"으로 처리되므로 분기 없이 넘긴다
	Instance->SetInstanceData(InstanceData);

	FIntPoint ItemSize(1, 1);
	if (const UGItemFragmentInventory* InvFrag = Instance->FindFragmentByClass<UGItemFragmentInventory>())
	{
		ItemSize = InvFrag->GridSize;
	}

	if (!AutoPlace(Handle, ItemSize))
	{
		UE_LOG(LogGSystem, Warning, TEXT("GrantItem 실패: 공간 없음 [%s]"), *ItemDef->GetName());
		return FInventoryItemHandle();
	}

	InventoryMap.Add(Handle, Instance);
	NotifyInventoryChanged();
	UE_LOG(LogGSystem, Log, TEXT("GrantItem 성공: 핸들 %u, 크기 %dx%d"), Handle.GetHandleId(), ItemSize.X, ItemSize.Y);
	return Handle;
}

UGItemInstance* UGInventoryComponent::GetItemByHandle(FInventoryItemHandle Handle) const
{
	if (!Handle.IsValid()) return nullptr;
	const TObjectPtr<UGItemInstance>* Found = InventoryMap.Find(Handle);
	return Found ? Found->Get() : nullptr;
}

FInventoryItemHandle UGInventoryComponent::GetHandleAt(int32 X, int32 Y) const
{
	const FInventoryItemHandle* Found = OccupiedSlots.Find(FIntPoint(X, Y));
	return Found ? *Found : FInventoryItemHandle();
}

UGItemInstance* UGInventoryComponent::GetItemAt(int32 X, int32 Y) const
{
	return GetItemByHandle(GetHandleAt(X, Y));
}

FIntPoint UGInventoryComponent::GetItemPosition(FInventoryItemHandle Handle) const
{
	const FInventoryGridEntry* Entry = GridEntries.Find(Handle);
	return Entry ? Entry->TopLeft : FIntPoint(-1, -1);
}

bool UGInventoryComponent::CanMoveItemTo(FInventoryItemHandle Handle, int32 TargetX, int32 TargetY) const
{
	if (!InventoryMap.Contains(Handle)) return false;
	return CanPlaceAt(GetItemSize(Handle), TargetX, TargetY, Handle);
}

bool UGInventoryComponent::CanPlaceNewItemAt(FIntPoint ItemSize, int32 StartX, int32 StartY) const
{
	return CanPlaceAt(ItemSize, StartX, StartY, FInventoryItemHandle());
}

bool UGInventoryComponent::HasSpaceForItem(FIntPoint ItemSize) const
{
	if (ItemSize.X <= 0 || ItemSize.Y <= 0) return false;

	for (int32 Y = 0; Y < Rows; ++Y)
	{
		for (int32 X = 0; X < Columns; ++X)
		{
			if (CanPlaceAt(ItemSize, X, Y, FInventoryItemHandle()))
			{
				return true;
			}
		}
	}
	return false;
}

bool UGInventoryComponent::MoveItem(FInventoryItemHandle Handle, int32 TargetX, int32 TargetY)
{
	if (!InventoryMap.Contains(Handle)) return false;

	const FIntPoint ItemSize = GetItemSize(Handle);
	if (!CanPlaceAt(ItemSize, TargetX, TargetY, Handle))
	{
		UE_LOG(LogGSystem, Warning, TEXT("MoveItem 실패: 핸들 %u → (%d, %d) 공간 없음"), Handle.GetHandleId(), TargetX, TargetY);
		return false;
	}
    
	// 만약 장착된(Equipped) 아이템이었다면 장비 슬롯에서 제거
	for (auto It = EquipmentSlots.CreateIterator(); It; ++It)
	{
		if (It.Value() == Handle)
		{
			It.RemoveCurrent();
			break;
		}
	}

	// ★ 만약 퀵슬롯에 등록되어 있던 아이템이었다면 퀵슬롯 참조 해제
	for (int32 i = 0; i < QuickSlots.Num(); ++i)
	{
		if (QuickSlots[i] == Handle)
		{
			QuickSlots[i] = FInventoryItemHandle(); 
		}
	}
    
	ClearSlotsForHandle(Handle);
	OccupySlots(Handle, ItemSize, TargetX, TargetY);

	NotifyInventoryChanged();
	UE_LOG(LogGSystem, Log, TEXT("MoveItem 성공: 핸들 %u → (%d, %d)"), Handle.GetHandleId(), TargetX, TargetY);
	return true;
}

bool UGInventoryComponent::RemoveItem(FInventoryItemHandle Handle)
{
	if (!InventoryMap.Contains(Handle)) return false;

	// 장비 슬롯에서 제거
	for (auto It = EquipmentSlots.CreateIterator(); It; ++It)
	{
		if (It.Value() == Handle)
		{
			It.RemoveCurrent();
			break;
		}
	}

	// 퀵슬롯에서 참조 해제
	for (int32 i = 0; i < QuickSlots.Num(); ++i)
	{
		if (QuickSlots[i] == Handle)
		{
			QuickSlots[i] = FInventoryItemHandle();
		}
	}

	ClearSlotsForHandle(Handle);
	InventoryMap.Remove(Handle);

	NotifyInventoryChanged();
	UE_LOG(LogGSystem, Log, TEXT("RemoveItem 성공: 핸들 %u"), Handle.GetHandleId());
	return true;
}

bool UGInventoryComponent::DropItem(FInventoryItemHandle Handle)
{
	TObjectPtr<UGItemInstance>* InstancePtr = InventoryMap.Find(Handle);
	if (!InstancePtr) return false;

	const UGItemDefinition* Definition = (*InstancePtr)->GetItemDef();
	if (!Definition) return false;

	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World) return false;

	TSubclassOf<AGItemPickup> SpawnClass = DropPickupClass;
	if (!SpawnClass) SpawnClass = AGItemPickup::StaticClass();

	const FVector SpawnLoc = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100.f;
	const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLoc);

	AGItemPickup* Pickup = World->SpawnActorDeferred<AGItemPickup>(
		SpawnClass, SpawnTransform, Owner, Cast<APawn>(Owner), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (!Pickup) return false;

	// TODO: 추후 스택 아이템 대응 시 Quantity 전달 구조 필요
	// 개체별 데이터를 함께 실어야 드롭했다 다시 주웠을 때 내용이 유지된다
	Pickup->InitializePickup(Definition, 1, (*InstancePtr)->GetInstanceDataStruct());
	Pickup->FinishSpawning(SpawnTransform);

	// 바닥에 버리면 인벤토리, 장비, 퀵슬롯 모두에서 제거됨
	RemoveItem(Handle);
	UE_LOG(LogGSystem, Log, TEXT("DropItem 성공: 핸들 %u 스폰 위치 %s"), Handle.GetHandleId(), *SpawnLoc.ToString());
	return true;
}

TArray<FInventoryItemHandle> UGInventoryComponent::GetAllHandles() const
{
	TArray<FInventoryItemHandle> Handles;
	InventoryMap.GetKeys(Handles);
	return Handles;
}

FInventoryItemHandle UGInventoryComponent::FindHandleByItemID(FName ItemID) const
{
	if (ItemID.IsNone()) return FInventoryItemHandle();

	for (const TPair<FInventoryItemHandle, TObjectPtr<UGItemInstance>>& Pair : InventoryMap)
	{
		const UGItemDefinition* Definition = Pair.Value ? Pair.Value->GetItemDef() : nullptr;
		if (Definition && Definition->ItemID == ItemID)
		{
			return Pair.Key;
		}
	}
	return FInventoryItemHandle();
}

FInventoryItemRenderData UGInventoryComponent::GetItemRenderData(FInventoryItemHandle Handle) const
{
	FInventoryItemRenderData RenderData;

	const FInventoryGridEntry* Entry = GridEntries.Find(Handle);
	if (!Entry) return RenderData;

	const TObjectPtr<UGItemInstance>* Found = InventoryMap.Find(Handle);
	if (!Found) return RenderData;

	RenderData.GridSize = Entry->Size;
	RenderData.Position = Entry->TopLeft;

	if (const UGItemFragmentInventory* InvFrag = (*Found)->FindFragmentByClass<UGItemFragmentInventory>())
		RenderData.Icon = InvFrag->ItemIcon;
	else
		RenderData.Icon = TSoftObjectPtr<UTexture2D>();

	// 개체별 아이콘이 있으면 그쪽이 우선 (사진은 찍힌 장면 자체가 아이콘이 된다)
	if (const FGItemInstanceData* InstanceData = (*Found)->GetInstanceData<FGItemInstanceData>())
	{
		RenderData.RuntimeIcon = InstanceData->GetRuntimeIcon();
	}

	RenderData.bIsValid = true;
	return RenderData;
}

// ─── 장비 및 퀵슬롯 로직 ────────────────────────────────────────────────────────

bool UGInventoryComponent::EquipItem(FInventoryItemHandle Handle, EEquipSlot Slot)
{
	if (!Handle.IsValid() || Slot == EEquipSlot::None) return false;
	if (!InventoryMap.Contains(Handle)) return false;

	// 이미 해당 부위에 장비가 있다면 해제 (실패 시 장착도 취소)
	if (EquipmentSlots.Contains(Slot) && EquipmentSlots[Slot].IsValid())
	{
		if (!UnequipItem(Slot)) return false; 
	}

	// 만약 장착하려는 아이템이 퀵슬롯에 등록되어 있던 거라면 퀵슬롯 비우기
	for (int32 i = 0; i < QuickSlots.Num(); ++i)
	{
		if (QuickSlots[i] == Handle)
		{
			QuickSlots[i] = FInventoryItemHandle();
			break;
		}
	}

	// 그리드에서 아이템 점유 해제
	ClearSlotsForHandle(Handle);
    
	EquipmentSlots.Add(Slot, Handle);
    
	NotifyInventoryChanged();
	G_LOG(TEXT("아이템 장착 성공: 부위 %d, 핸들 %u"), (int32)Slot, Handle.GetHandleId());
	return true;
}

bool UGInventoryComponent::UnequipItem(EEquipSlot Slot, int32 TargetX, int32 TargetY)
{
	if (!EquipmentSlots.Contains(Slot)) return false;
	
	FInventoryItemHandle Handle = EquipmentSlots[Slot];
	if (!Handle.IsValid()) return false;

	const FIntPoint ItemSize = GetItemSize(Handle);
	bool bPlaced = false;

	// 특정 좌표 지정 시 배치 시도
	if (TargetX >= 0 && TargetY >= 0 && CanPlaceAt(ItemSize, TargetX, TargetY, FInventoryItemHandle()))
	{
		OccupySlots(Handle, ItemSize, TargetX, TargetY);
		bPlaced = true;
	}
	// 자동 배치 시도
	else if (AutoPlace(Handle, ItemSize))
	{
		bPlaced = true;
	}

	if (bPlaced)
	{
		EquipmentSlots.Remove(Slot);
		NotifyInventoryChanged();
		G_LOG(TEXT("아이템 장착 해제 성공: 핸들 %u"), Handle.GetHandleId());
		return true;
	}

	G_WARN(TEXT("아이템 장착 해제 실패: 인벤토리에 공간이 없습니다."));
	return false;
}

bool UGInventoryComponent::AssignQuickSlot(FInventoryItemHandle Handle, int32 SlotIndex)
{
	// 만약 Handle이 유효하지 않다면(비우기 요청인 경우), 특정 슬롯만 비우고 종료합니다.
	if (!Handle.IsValid())
	{
		if (QuickSlots.IsValidIndex(SlotIndex))
		{
			QuickSlots[SlotIndex] = FInventoryItemHandle();
			NotifyInventoryChanged();
			return true;
		}
		return false;
	}

	if (!InventoryMap.Contains(Handle)) return false;
	if (!QuickSlots.IsValidIndex(SlotIndex)) return false;

	// 1. 만약 이 아이템이 이미 다른 퀵슬롯에 들어가 있던 거라면, 그 자리 비우기
	for (int32 i = 0; i < QuickSlots.Num(); ++i)
	{
		if (QuickSlots[i] == Handle)
		{
			QuickSlots[i] = FInventoryItemHandle(); // 빈 핸들 대입으로 초기화
			break;
		}
	}

	// 2. 만약 이 아이템이 장비 슬롯(EquipmentSlots)에 장착되어 있던 거라면 장비 해제
	for (auto It = EquipmentSlots.CreateIterator(); It; ++It)
	{
		if (It.Value() == Handle)
		{
			It.RemoveCurrent();
			break;
		}
	}

	// 3. 인벤토리 그리드(GridEntries)에 있던 아이템이라면 그리드 점유 해제
	ClearSlotsForHandle(Handle);

	// 4. 새로운 퀵슬롯 인덱스에 등록
	QuickSlots[SlotIndex] = Handle;

	NotifyInventoryChanged();
	return true;
}

void UGInventoryComponent::ClearQuickSlot(int32 SlotIndex)
{
	AssignQuickSlot(FInventoryItemHandle(), SlotIndex);
}

FInventoryItemHandle UGInventoryComponent::GetEquippedItem(EEquipSlot Slot) const
{
	const FInventoryItemHandle* Found = EquipmentSlots.Find(Slot);
	return Found ? *Found : FInventoryItemHandle();
}

FInventoryItemHandle UGInventoryComponent::GetQuickSlotItem(int32 SlotIndex) const
{
	if (QuickSlots.IsValidIndex(SlotIndex))
	{
		return QuickSlots[SlotIndex];
	}
	return FInventoryItemHandle();
}

// ─── 비공개 헬퍼 ──────────────────────────────────────────────────────────────

bool UGInventoryComponent::CanPlaceAt(FIntPoint ItemSize, int32 StartX, int32 StartY, FInventoryItemHandle ExcludeHandle) const
{
	for (int32 DY = 0; DY < ItemSize.Y; ++DY)
	{
		for (int32 DX = 0; DX < ItemSize.X; ++DX)
		{
			const int32 CX = StartX + DX;
			const int32 CY = StartY + DY;

			if (CX < 0 || CX >= Columns || CY < 0 || CY >= Rows) return false;

			const FInventoryItemHandle* CellHandle = OccupiedSlots.Find(FIntPoint(CX, CY));
			if (CellHandle && CellHandle->IsValid() && !(*CellHandle == ExcludeHandle))
			{
				return false;
			}
		}
	}
	return true;
}

void UGInventoryComponent::OccupySlots(FInventoryItemHandle Handle, FIntPoint ItemSize, int32 StartX, int32 StartY)
{
	for (int32 DY = 0; DY < ItemSize.Y; ++DY)
	{
		for (int32 DX = 0; DX < ItemSize.X; ++DX)
		{
			OccupiedSlots.Add(FIntPoint(StartX + DX, StartY + DY), Handle);
		}
	}

	FInventoryGridEntry Entry;
	Entry.TopLeft = FIntPoint(StartX, StartY);
	Entry.Size    = ItemSize;
	GridEntries.Add(Handle, Entry);
}

void UGInventoryComponent::ClearSlotsForHandle(FInventoryItemHandle Handle)
{
	const FInventoryGridEntry* Entry = GridEntries.Find(Handle);
	if (!Entry) return;

	for (int32 DY = 0; DY < Entry->Size.Y; ++DY)
	{
		for (int32 DX = 0; DX < Entry->Size.X; ++DX)
		{
			OccupiedSlots.Remove(FIntPoint(Entry->TopLeft.X + DX, Entry->TopLeft.Y + DY));
		}
	}
	GridEntries.Remove(Handle);
}

bool UGInventoryComponent::AutoPlace(FInventoryItemHandle Handle, FIntPoint ItemSize)
{
	for (int32 Y = 0; Y < Rows; ++Y)
	{
		for (int32 X = 0; X < Columns; ++X)
		{
			if (CanPlaceAt(ItemSize, X, Y, FInventoryItemHandle()))
			{
				OccupySlots(Handle, ItemSize, X, Y);
				return true;
			}
		}
	}
	return false;
}

FIntPoint UGInventoryComponent::GetItemSize(FInventoryItemHandle Handle) const
{
	// 아이템이 장착된 상태이거나 Fragment에서 원본 크기를 가져오는 로직도 필요시 추가 가능
	const FInventoryGridEntry* Entry = GridEntries.Find(Handle);
	if (Entry) return Entry->Size;

	// Grid에 없다면 (장비창) Fragment를 긁어오기
	const TObjectPtr<UGItemInstance>* Found = InventoryMap.Find(Handle);
	if (Found && *Found)
	{
		if (const UGItemFragmentInventory* InvFrag = (*Found)->FindFragmentByClass<UGItemFragmentInventory>())
		{
			return InvFrag->GridSize;
		}
	}

	return FIntPoint(1, 1);
}

void UGInventoryComponent::NotifyInventoryChanged()
{
	OnInventoryChanged.Broadcast();
}

// ─── 저장/불러오기 ────────────────────────────────────────────────────────────

void UGInventoryComponent::ExportInventorySaveData(TArray<FGuestSavedInventoryEntry>& OutEntries) const
{
	OutEntries.Reset();
	
	const TArray<FInventoryItemHandle> Handles = GetAllHandles();
	OutEntries.Reserve(Handles.Num());
	
	for (const FInventoryItemHandle& Handle : Handles)
	{
		if (!Handle.IsValid()) continue;
		
		const UGItemInstance* Instance = GetItemByHandle(Handle);
		if (!Instance) continue;
		
		const UGItemDefinition* ItemDef = Instance->GetItemDef();
		if (!ItemDef || ItemDef->ItemID.IsNone()) continue;
		
		const FIntPoint TopLeft = GetItemPosition(Handle);
		
		// 추후 Save 구조체에 장착 상태나 퀵슬롯 저장 여부도 확장해야 완벽하게 복구됨. 
		// (현재는 Grid 위에 있는 아이템만 저장)
		if (TopLeft.X < 0 || TopLeft.Y < 0) continue;
		
		FGuestSavedInventoryEntry Entry;
		Entry.ItemID       = ItemDef->ItemID;
		Entry.TopLeft      = TopLeft;
		Entry.Size         = GetItemSize(Handle);
		Entry.Quantity     = 1;  // 스택 없으면 항상 1
		Entry.InstanceData = Instance->GetInstanceDataStruct();  // 없으면 빈 구조체

		OutEntries.Add(MoveTemp(Entry));
	}
}

void UGInventoryComponent::ClearInventory()
{
	OccupiedSlots.Empty();
	GridEntries.Empty();
	InventoryMap.Empty();
	EquipmentSlots.Empty();
	QuickSlots.Init(FInventoryItemHandle(), 4);
}

//그리드 좌표에 아이템을 배치하는 함수.
bool UGInventoryComponent::PlaceItemAt(const UGItemDefinition* ItemDef, FIntPoint TopLeft, FIntPoint Size,
	const FInstancedStruct& InstanceData)
{
	if (!ItemDef) return false;
	if (Size.X <= 0 || Size.Y <= 0) return false;
	if (!CanPlaceAt(Size, TopLeft.X, TopLeft.Y, FInventoryItemHandle())) return false;
	
	const FInventoryItemHandle Handle = FInventoryItemHandle::CreateHandle();
	UGItemInstance* Instance = NewObject<UGItemInstance>(this);
	Instance->InitInstance(Handle, ItemDef);
	Instance->SetInstanceData(InstanceData);

	OccupySlots(Handle, Size, TopLeft.X, TopLeft.Y);
	InventoryMap.Add(Handle, Instance);

	return true;
}

void UGInventoryComponent::ImportInventorySaveData(const TArray<FGuestSavedInventoryEntry>& InEntries)
{
	ClearInventory();
	
	for (const FGuestSavedInventoryEntry& Entry : InEntries)
	{
		if (Entry.ItemID.IsNone()) continue;
		if (Entry.TopLeft.X < 0 || Entry.TopLeft.Y < 0) continue;
		if (Entry.Size.X <= 0 || Entry.Size.Y <= 0) continue;
		
		const UGItemDefinition* ItemDef = FindItemDefinitionByItemID(Entry.ItemID);
		if (!ItemDef)
		{
			G_WARN(TEXT("ImportInventory 스킵: ItemID [%s] 없음"), *Entry.ItemID.ToString());
			continue;
		}
		if (!PlaceItemAt(ItemDef, Entry.TopLeft, Entry.Size, Entry.InstanceData))
		{
			G_WARN(TEXT("ImportInventory 스킵: [%s] (%d,%d) 배치 실패"),
				*Entry.ItemID.ToString(), Entry.TopLeft.X, Entry.TopLeft.Y);
			continue;
		}
	}
	NotifyInventoryChanged();
}