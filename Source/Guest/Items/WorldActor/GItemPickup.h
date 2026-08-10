// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "StructUtils/InstancedStruct.h"
#include "GItemPickup.generated.h"

class UGItemDefinition;

UCLASS()
class GUEST_API AGItemPickup : public AActor, public IGInteractableInterface
{
	GENERATED_BODY()

public:
	AGItemPickup();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<const UGItemDefinition> ItemDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 Quantity = 1;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractText_Implementation() const override;

	// 개체별 데이터 (사진의 촬영 내용 등). 인벤토리에서 드롭된 아이템은 이걸 물고 나온다.
	// 레벨에 미리 배치된 픽업은 보통 비어 있다.
	UPROPERTY(VisibleInstanceOnly, Category = "Item")
	FInstancedStruct InstanceData;

	// 드롭 스폰 시 Definition + 수량 + 개체별 데이터로 초기화.
	// InInstanceData를 넘기지 않으면 개체 데이터 없는 일반 아이템이 된다.
	void InitializePickup(const UGItemDefinition* InDefinition, int32 InQuantity = 1,
		const FInstancedStruct& InInstanceData = FInstancedStruct());

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void UpdatePickupVisuals() const;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;
};