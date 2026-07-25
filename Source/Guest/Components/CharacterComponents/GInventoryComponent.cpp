// Copyright (c) 2026 Anything Left Behind?. All rights reserved.
// test: PR Actions 연동 테스트 2


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

		// GuestAssetManager가 등록된 경우 Primary Asset 목록 우선 사용
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

		// Fallback: Asset Registry (Fatal 없이 ItemID 검색)
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
	UE_LOG(LogGSystem, Log, TEXT("인벤토리 초기화: %d x %d (%d 슬롯)"), Columns, Rows, Columns * Rows);
}

// ─── 공개 API ────────────────────────────────────────────────────────────────

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

	ClearSlotsForHandle(Handle);
	OccupySlots(Handle, ItemSize, TargetX, TargetY);

	NotifyInventoryChanged();
	UE_LOG(LogGSystem, Log, TEXT("MoveItem 성공: 핸들 %u → (%d, %d)"), Handle.GetHandleId(), TargetX, TargetY);
	return true;
}

bool UGInventoryComponent::RemoveItem(FInventoryItemHandle Handle)
{
	if (!InventoryMap.Contains(Handle)) return false;

	ClearSlotsForHandle(Handle);
	InventoryMap.Remove(Handle);

	NotifyInventoryChanged();
	UE_LOG(LogGSystem, Log, TEXT("RemoveItem 성공: 핸들 %u"), Handle.GetHandleId());
	return true;
}

bool UGInventoryComponent::DropItem(FInventoryItemHandle Handle)
{
	TObjectPtr<UGItemInstance>* InstancePtr = InventoryMap.Find(Handle);
	if (!InstancePtr)
	{
		UE_LOG(LogGSystem, Warning, TEXT("DropItem 실패: 핸들 %u 인스턴스 없음"), Handle.GetHandleId());
		return false;
	}

	const UGItemDefinition* Definition = (*InstancePtr)->GetItemDef();
	if (!Definition)
	{
		UE_LOG(LogGSystem, Warning, TEXT("DropItem 실패: 핸들 %u 아이템 정의 없음"), Handle.GetHandleId());
		return false;
	}

	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		UE_LOG(LogGSystem, Warning, TEXT("DropItem 실패: Owner 또는 World 없음, 핸들 %u"), Handle.GetHandleId());
		return false;
	}

	// DropPickupClass 미설정 시 기본 AGItemPickup 사용
	TSubclassOf<AGItemPickup> SpawnClass = DropPickupClass;

	if (!SpawnClass)
	{
		SpawnClass = AGItemPickup::StaticClass();
	}

	const FVector SpawnLoc = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100.f;
	const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLoc);

	// SpawnActorDeferred: InitializePickup이 BeginPlay보다 먼저 실행되도록 보장
	AGItemPickup* Pickup = World->SpawnActorDeferred<AGItemPickup>(
		SpawnClass,
		SpawnTransform,
		Owner,
		Cast<APawn>(Owner),
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (!Pickup)
	{
		UE_LOG(LogGSystem, Error, TEXT("DropItem: SpawnActorDeferred 실패, 핸들 %u"), Handle.GetHandleId());
		return false;
	}

	// TODO: 추후 스택 아이템 대응 시 Quantity 전달 구조 필요
	// 개체별 데이터를 함께 실어야 드롭했다 다시 주웠을 때 내용이 유지된다
	Pickup->InitializePickup(Definition, 1, (*InstancePtr)->GetInstanceDataStruct());
	Pickup->FinishSpawning(SpawnTransform);

	// 스폰 성공 후에만 인벤토리에서 제거
	ClearSlotsForHandle(Handle);
	InventoryMap.Remove(Handle);
	NotifyInventoryChanged();

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
	if (!Entry) return RenderData;  // bIsValid = false

	// GridEntry는 있는데 Instance가 없으면 데이터 불일치 → 렌더링 안 함
	const TObjectPtr<UGItemInstance>* Found = InventoryMap.Find(Handle);
	if (!Found) return RenderData;  // bIsValid = false

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
	const FInventoryGridEntry* Entry = GridEntries.Find(Handle);
	return Entry ? Entry->Size : FIntPoint(1, 1);
}

void UGInventoryComponent::NotifyInventoryChanged()
{
	OnInventoryChanged.Broadcast();
}

void UGInventoryComponent::ExportInventorySaveData(TArray<FGuestSavedInventoryEntry>& OutEntries) const
{
	OutEntries.Reset();
	
	const TArray<FInventoryItemHandle> Handles = GetAllHandles();
	OutEntries.Reserve(Handles.Num());
	
	for (const FInventoryItemHandle& Handle : Handles)
	{
		if (!Handle.IsValid())
		{
			continue;
		}
		
		const UGItemInstance* Instance = GetItemByHandle(Handle);
		if (!Instance)
		{
			continue;
		}
		
		const UGItemDefinition* ItemDef = Instance->GetItemDef();
		if (!ItemDef || ItemDef->ItemID.IsNone())
		{
			continue;
		}
		
		const FIntPoint TopLeft = GetItemPosition(Handle);
		if (TopLeft.X < 0 || TopLeft.Y < 0)
		{
			continue;
		}
		
		FGuestSavedInventoryEntry Entry;
		Entry.ItemID       = ItemDef->ItemID;
		Entry.TopLeft      = TopLeft;
		Entry.Size         = GetItemSize(Handle);
		Entry.Quantity     = 1;  // 스택 없으면 항상 1
		Entry.InstanceData = Instance->GetInstanceDataStruct();  // 없으면 빈 구조체

		OutEntries.Add(MoveTemp(Entry));
	}
	// UE_LOG(LogGSystem, Log, TEXT("ExportInventory: %d items"), OutEntries.Num());
}

void UGInventoryComponent::ClearInventory()
{
	OccupiedSlots.Empty();
	GridEntries.Empty();
	InventoryMap.Empty();
}

//그리드 좌표에 아이템을 배치하는 함수.
bool UGInventoryComponent::PlaceItemAt(const UGItemDefinition* ItemDef, FIntPoint TopLeft, FIntPoint Size,
	const FInstancedStruct& InstanceData)
{
	if (!ItemDef)
	{
		return false;
	}
	if (Size.X <= 0 || Size.Y <= 0)
	{
		return false;
	}
	if (!CanPlaceAt(Size, TopLeft.X, TopLeft.Y, FInventoryItemHandle()))
	{
		return false;
	}
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
