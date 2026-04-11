// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GuestGameplayTypes.Generated.h"

USTRUCT( BlueprintType )
struct FGuestCharacterBaseStats : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;
	
	UPROPERTY(EditAnywhere)
	float Health;
	
	UPROPERTY(EditAnywhere)
	float Battery;
};