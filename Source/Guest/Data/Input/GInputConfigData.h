// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GInputConfigData.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FGuestInputConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly,meta = (Categories = "Guest.InputTag"))
	FGameplayTag InputTag;
	
	UPROPERTY(EditDefaultsOnly)
	UInputAction* InputAction;
	
};
/**
 * 
 */
UCLASS()
class GUEST_API UGInputConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FGuestInputConfig> AbilityInputActions;
};
