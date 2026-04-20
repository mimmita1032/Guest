#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GuestSaveGame.generated.h"


USTRUCT(BlueprintType)
struct FGuestPlayerWorldState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FRotator Rotation = FRotator::ZeroRotator;
};


UCLASS()
class GUEST_API UGuestSaveGame : public USaveGame
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save|Meta")
	int32 SaveVersion = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Meta")
	FDateTime SavedAt;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|World")
	FString MapPackageName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Player")
	FGuestPlayerWorldState PlayerWorld;
};