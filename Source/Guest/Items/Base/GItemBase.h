// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "GItemBase.generated.h"

/**
 * 프로젝트 내의 모든 상호작용 가능한 아이템의 최상위 부모 클래스
 */
UCLASS()
class GUEST_API AGItemBase : public AActor, public IGInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AGItemBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionText;

public:
	virtual void Interact(AActor* Interactor) override;
	virtual FText GetInteractText() const override;
,};`feaf 3`