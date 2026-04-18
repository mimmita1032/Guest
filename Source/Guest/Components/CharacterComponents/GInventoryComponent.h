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

private:
	// 아이템 인스턴스 보관 배열
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory State")
	TArray<TObjectPtr<UGItemInstance>> InventorySlots;
};
