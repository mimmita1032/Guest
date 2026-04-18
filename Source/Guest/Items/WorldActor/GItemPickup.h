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
	
	//데이터 에셋 결정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<const UGItemDefinition> ItemDefinition;
	
	// Pickup 함수 구현
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractText_Implementation() const override;

protected:
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	void UpdatePickupVisuals() const;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;
};
