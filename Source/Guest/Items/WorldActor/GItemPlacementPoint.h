// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "GItemPlacementPoint.generated.h"

class UGItemDefinition;
class UBoxComponent;
class UStaticMeshComponent;

// 인벤토리의 특정 아이템을 레벨의 지정된 위치(테이블 등)에 놓는 상호작용 지점.
// AGItemPickup의 반대 방향 동작: 인벤토리 → 월드 배치.
UCLASS()
class GUEST_API AGItemPlacementPoint : public AActor, public IGInteractableInterface
{
	GENERATED_BODY()

public:
	AGItemPlacementPoint();

	// 이 지점에 놓아야 하는 아이템
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<const UGItemDefinition> RequiredItem;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractText_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;

	// 배치 완료 후에만 보여지는 아이템 시각화
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PlacedMeshComp;

	// 이미 배치가 끝났는지 (중복 배치 방지)
	UPROPERTY(VisibleInstanceOnly, Category = "Item")
	bool bIsPlaced = false;
};
