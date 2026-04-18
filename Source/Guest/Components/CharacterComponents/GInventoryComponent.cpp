// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GInventoryComponent.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "Guest/Items/Instance/GItemInstance.h"

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