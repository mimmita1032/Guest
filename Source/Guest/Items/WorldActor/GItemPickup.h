// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Guest/Interfaces/GInteractableInterface.h"
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

	// 드롭 스폰 시 Definition + 수량으로 초기화
	void InitializePickup(const UGItemDefinition* InDefinition, int32 InQuantity = 1);

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void UpdatePickupVisuals() const;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;
};