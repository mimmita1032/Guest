// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractTextChanged, const FText&, NewText);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUEST_API UGInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGInteractionComponent();

	void DoInteract();

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractTextChanged OnInteractTextChanged;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AActor* FindInteractable() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionRange = 300.0f;

	UPROPERTY(VisibleInstanceOnly, Category = "Interaction")
	TObjectPtr<AActor> FocusedActor;

private:
	FText CurrentInteractText;

	bool bIsPromptPushed = false;
};