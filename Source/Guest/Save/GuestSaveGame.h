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

USTRUCT(BlueprintType)
struct FGuestSavedActiveQuestEntry
{
	GENERATED_BODY()
	// 예: Q_Main_001
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	FName QuestID = NAME_None;
	// 현재 진행 단계 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	int32 CurrentStep = 0;
	// 각 목표 누적 수치 (Objectives와 인덱스 대응)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	TArray<int32> ObjectiveCounts;
};


UCLASS()
class GUEST_API UGuestSaveGame : public USaveGame
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save|Meta")
	int32 SaveVersion = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Meta")
	FDateTime SavedAt;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|World")
	FString MapPackageName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Player")
	FGuestPlayerWorldState PlayerWorld;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	TArray<FGuestSavedActiveQuestEntry> SavedActiveQuests;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	TArray<FName> SavedCompletedQuestIDs;
};