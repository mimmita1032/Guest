// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GInteractionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUEST_API UGInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGInteractionComponent();

	void DoInteract();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AActor* FindInteractable() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionRange = 300.0f;

	UPROPERTY(VisibleInstanceOnly, Category = "Interaction")
	TObjectPtr<AActor> FocusedActor;
};