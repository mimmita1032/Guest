// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "GItemPlacementPoint.generated.h"

class UGItemDefinition;
class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

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
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;

	// 배치 전엔 반투명 미리보기로, 배치 후엔 정상 머티리얼로 보여지는 아이템 시각화
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PlacedMeshComp;

	// 배치 전 미리보기용 반투명 머티리얼 — 에디터에서 지정 (AGItemPlacementGhost와 동일한 M_Ghost 사용 권장)
	UPROPERTY(EditAnywhere, Category = "Placement")
	TObjectPtr<UMaterialInterface> GhostMaterial;

	// 이미 배치가 끝났는지 (중복 배치 방지)
	UPROPERTY(VisibleInstanceOnly, Category = "Item")
	bool bIsPlaced = false;

private:
	void UpdatePreviewVisuals();

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicGhostMaterial;
};
