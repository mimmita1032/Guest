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
	virtual void BeginPlay() override;
	
	// 아이템 외형
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;
	
	// 아이템 정보를 담고 있는 Data Asset
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	TObjectPtr<UGItemData> ItemData;
	
	// 상호작용 시 UI에 표시될 텍스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionText;

public:
	// IGInteractableInterface 구현
	virtual void Interact(AActor* Interactor) override;
	virtual FText GetInteractText() const override;
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	UGItemData* GetItemData() const { return ItemData; }
};