// Copyright (c) 2026 Anything Left Behind?. All rights reserved.
// test: PR Actions 연동 테스트

#include "GInventoryComponent.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "Guest/Items/WorldActor/GItemPickup.h"
#include "Guest/Utils/GLog.h"

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
	if (!ItemDef) return FInventoryItemHandle();

	const FInventoryItemHandle Handle = FInventoryItemHandle::CreateHandle();
	UGItemInstance* Instance = NewObject<UGItemInstance>(this);
	Instance->InitInstance(Handle, ItemDef);

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
	Pickup->InitializePickup(Definition, 1);
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