// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GInventoryComponent.generated.h"

class UGItemInstance;

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

private:
	// 아이템 인스턴스 보관 배열
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory State")
	TArray<TObjectPtr<UGItemInstance>> InventorySlots;
};
