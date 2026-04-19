// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GInventoryComponent.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "Guest/Items/WorldActor/GItemPickup.h"

UGInventoryComponent::UGInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false; 
}

void UGInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// 총 슬롯 개수 계산
	const int32 TotalSlots = Columns * Rows;

	// 배열 초기화 
	InventorySlots.Init(nullptr, TotalSlots);

	// 초기화 결과
	UE_LOG(LogGSystem, Log, TEXT("인벤토리 컴포넌트 초기화 완료. 크기: %d x %d (총 %d 슬롯)"), Columns, Rows, TotalSlots);
}

int32 UGInventoryComponent::GetIndex(int32 X, int32 Y) const
{
	// 그리드 범위를 벗어나면 유효하지 않은 인덱스(-1) 반환
	if (X < 0 || X >= Columns || Y < 0 || Y >= Rows)
	{
		return -1;
	}
	// 2D 좌표를 1D 인덱스로 변환
	return (Y * Columns) + X;
}

bool UGInventoryComponent::IsValidIndex(int32 Index) const
{
	return InventorySlots.IsValidIndex(Index);
}

bool UGInventoryComponent::IsSlotEmpty(int32 Index) const
{
	// 인덱스가 범위를 벗어났다면 사용 불가
	if (!IsValidIndex(Index))
	{
		return false;
	}
	// nullptr이면 빈 공간
	return InventorySlots[Index] == nullptr;
}

bool UGInventoryComponent::CanAddItemAt(UGItemInstance* ItemInstance, int32 StartX, int32 StartY) const
{
	if (!ItemInstance)
	{
		return false;
	}

	// 프래그먼트에서 아이템의 (가로, 세로) 크기 추출 (기본값 1x1)
	FIntPoint ItemSize = FIntPoint(1, 1);
	if (const UGItemFragmentInventory* InvFrag = ItemInstance->FindFragmentByClass<UGItemFragmentInventory>())
	{
		ItemSize = InvFrag->GridSize;
	}

	// 가로, 세로 크기만큼 반복문을 돌며 차지할 공간이 모두 비어있는지
	for (int32 X = StartX; X < StartX + ItemSize.X; ++X)
	{
		for (int32 Y = StartY; Y < StartY + ItemSize.Y; ++Y)
		{
			const int32 Index = GetIndex(X, Y);

			// 칸이 가방 범위를 벗어났거나, 이미 다른 아이템이 들어있다면 실패
			// 나중에 그 자리를 대체할 수 있게 리팩토링 예정!

			if (!IsValidIndex(Index) || !IsSlotEmpty(Index))
			{
				return false;
			}
		}
	}

	return true;
}

bool UGInventoryComponent::AddItemAt(UGItemInstance* ItemInstance, int32 StartX, int32 StartY)
{
	if (!CanAddItemAt(ItemInstance, StartX, StartY))
	{
		UE_LOG(LogGSystem, Warning, TEXT("아이템 추가 실패: 공간이 부족하거나 범위를 벗어남. 좌표(%d, %d)"), StartX, StartY);
		return false;
	}

	FIntPoint ItemSize = FIntPoint(1, 1);
	if (const UGItemFragmentInventory* InvFrag = ItemInstance->FindFragmentByClass<UGItemFragmentInventory>())
	{
		ItemSize = InvFrag->GridSize;
	}

	for (int32 X = StartX; X < StartX + ItemSize.X; ++X)
	{
		for (int32 Y = StartY; Y < StartY + ItemSize.Y; ++Y)
		{
			const int32 Index = GetIndex(X, Y);
			InventorySlots[Index] = ItemInstance; // 빈 공간(nullptr)을 아이템 객체로 덮어씌움
		}
	}

	UE_LOG(LogGSystem, Log, TEXT("아이템 추가 성공: 좌표(%d, %d), 크기(%dx%d)"), StartX, StartY, ItemSize.X, ItemSize.Y);
	
	OnInventoryChanged.Broadcast();
	
	return true;
}

bool UGInventoryComponent::RemoveItem(UGItemInstance* ItemToRemove)
{
	if (!ItemToRemove)
	{
		return false;
	}

	bool bRemoved = false;

	for (int32 i = 0; i < InventorySlots.Num(); ++i)
	{
		if (InventorySlots[i] == ItemToRemove)
		{
			InventorySlots[i] = nullptr;
			bRemoved = true;
		}
	}

	if (bRemoved)
	{
		UE_LOG(LogGSystem, Log, TEXT("아이템 제거 성공. 가방 공간 확보 완료."));
		OnInventoryChanged.Broadcast();
		return true;
	}

	UE_LOG(LogGSystem, Warning, TEXT("아이템 제거 실패: 가방에 존재하지 않는 아이템입니다."));
	return false;
}

bool UGInventoryComponent::AutoAddItem(UGItemInstance* ItemInstance)
{
	if (!ItemInstance) return false;

	// (0,0)부터 모든 칸을 순회하며 들어갈 자리가 있는지 확인
	for (int32 Y = 0; Y < Rows; ++Y)
	{
		for (int32 X = 0; X < Columns; ++X)
		{
			// CanAddItemAt이 아이템의 GridSize(1x2, 2x2 등)를 고려해서 여유 공간이 있는지 검사해 줌
			if (CanAddItemAt(ItemInstance, X, Y))
			{
				return AddItemAt(ItemInstance, X, Y);
			}
		}
	}

	UE_LOG(LogGSystem, Warning, TEXT("가방이 가득 차서 아이템을 획득할 수 없습니다!"));
	return false;
}

UGItemInstance* UGInventoryComponent::GetItemAt(int32 X, int32 Y) const
{
	int32 Index = Y * Columns + X;
	
	if (InventorySlots.IsValidIndex(Index))
	{
		return InventorySlots[Index];
	}
    
	return nullptr;
}

bool UGInventoryComponent::MoveItem(UGItemInstance* ItemToMove, int32 TargetX, int32 TargetY)
{
	if (!ItemToMove) return false;

	TArray<int32> OldIndices;
	for (int32 i = 0; i < InventorySlots.Num(); ++i)
	{
		if (InventorySlots[i] == ItemToMove)
		{
			OldIndices.Add(i);
			InventorySlots[i] = nullptr;
		}
	}

	if (CanAddItemAt(ItemToMove, TargetX, TargetY))
	{
		FIntPoint ItemSize = FIntPoint(1, 1);
		if (const UGItemFragmentInventory* InvFrag = ItemToMove->FindFragmentByClass<UGItemFragmentInventory>())
		{
			ItemSize = InvFrag->GridSize;
		}

		for (int32 X = TargetX; X < TargetX + ItemSize.X; ++X)
		{
			for (int32 Y = TargetY; Y < TargetY + ItemSize.Y; ++Y)
			{
				InventorySlots[GetIndex(X, Y)] = ItemToMove;
			}
		}

		OnInventoryChanged.Broadcast();
		UE_LOG(LogGSystem, Log, TEXT("아이템 이동 성공: 타겟 좌표(%d, %d)"), TargetX, TargetY);
		return true;
	}
	else
	{
		//공간이 부족하거나 범위를 벗어나면, 원래 기억해둔 자리로 롤백
		for (int32 OldIndex : OldIndices)
		{
			InventorySlots[OldIndex] = ItemToMove;
		}
		
		// 드래그하다가 실패했을 때 원본의 반투명을 해제
		OnInventoryChanged.Broadcast(); 
		UE_LOG(LogGSystem, Warning, TEXT("아이템 이동 실패: 타겟 좌표(%d, %d)에 공간이 부족합니다."), TargetX, TargetY);
		return false;
	}
}

bool UGInventoryComponent::DropItem(UGItemInstance* ItemToDrop)
{
	if (!ItemToDrop) return false;

	// 배열 탐색 및 인벤토리 슬롯 초기화
	bool bRemoved = false;
	for (int32 i = 0; i < InventorySlots.Num(); ++i)
	{
		if (InventorySlots[i] == ItemToDrop)
		{
			InventorySlots[i] = nullptr;
			bRemoved = true;
		}
	}

	if (bRemoved)
	{
		OnInventoryChanged.Broadcast();

		if (AActor* OwnerActor = GetOwner())
		{
			FVector SpawnLocation = OwnerActor->GetActorLocation() + (OwnerActor->GetActorForwardVector() * 100.0f);
			FRotator SpawnRotation = FRotator::ZeroRotator;

			if (UWorld* World = GetWorld())
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				if (AGItemPickup* DroppedItem = World->SpawnActor<AGItemPickup>(AGItemPickup::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams))
				{
					DroppedItem->InitializePickup(ItemToDrop);
					UE_LOG(LogGSystem, Log, TEXT("아이템 월드 드롭 완료. 데이터 인스턴스 전달 대기"));
					return true;
				}
			}
		}
	}

	return false;
}