// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GInventoryComponent.generated.h"

class UGItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUEST_API UGInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 가로 칸 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Settings")
	int32 Columns = 10;

	// 세로 칸 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Settings")
	int32 Rows = 5;

	// 2D 좌표(X, Y)를 1차원 배열 인덱스로 변환
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Grid")
	int32 GetIndex(int32 X, int32 Y) const;

	// 해당 인덱스가 유효한 배열 범위 내에 있는지 확인
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Grid")
	bool IsValidIndex(int32 Index) const;

	// 특정 인덱스의 슬롯이 비어있는지(nullptr) 확인
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Grid")
	bool IsSlotEmpty(int32 Index) const;

	// 아이템을 지정된 자리?에 (X, Y) 배치할 여유 공간이 있는지
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	bool CanAddItemAt(UGItemInstance* ItemInstance, int32 StartX, int32 StartY) const;

	// 특정 위치에 아이템을 실제로 추가 (성공 하면ㄴ true 반환)
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	bool AddItemAt(UGItemInstance* ItemInstance, int32 StartX, int32 StartY);

	// 첫 번째로 가능한 빈칸을 찾아서 자동으로 넣기
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	bool AutoAddItem(UGItemInstance* ItemInstance);

	// 인벤토리에서 특정 아이템을 완전히 제거하고 해당 공간들을 비움
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	bool RemoveItem(UGItemInstance* ItemToRemove);

	// 블루프린트 위젯에서 이벤트를 바인딩하여 UI를 갱신할 수 있도록 노출된 델리게이트 변수
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryChanged OnInventoryChanged;

	// 특정 좌표(X, Y)의 아이템 데이터를 가져오는 함수
	UFUNCTION(BlueprintPure, Category = "Inventory|Grid")
	class UGItemInstance* GetItemAt(int32 X, int32 Y) const;

	// 특정 위치의 아이템을 새로운 좌표로 이동
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	bool MoveItem(UGItemInstance* ItemToMove, int32 TargetX, int32 TargetY);

	// 지정된 아이템을 인벤토리에서 제거하고 월드에 다시 버리기
	UFUNCTION(BlueprintCallable, Category = "Inventory|Action")
	bool DropItem(UGItemInstance* ItemToDrop);
	
	

private:
	// 아이템 인스턴스 보관 배열
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory State")
	TArray<TObjectPtr<UGItemInstance>> InventorySlots;
};
