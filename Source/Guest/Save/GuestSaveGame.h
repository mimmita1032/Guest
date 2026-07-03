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

USTRUCT(BlueprintType)
struct FGuestSavedInventoryEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	FName ItemID = NAME_None;
	
	//아이템 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	FIntPoint TopLeft = FIntPoint::ZeroValue;
	
	//아이템 사이즈
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	FIntPoint Size = FIntPoint(1, 1);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	int32 Quantity = 1;
};

UCLASS()
class GUEST_API UGuestSaveGame : public USaveGame
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save|Meta")
	int32 SaveVersion = 4;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Meta")
	FDateTime SavedAt;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|World")
	FString MapPackageName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Player")
	FGuestPlayerWorldState PlayerWorld;
	
	// 퀘스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	TArray<FGuestSavedActiveQuestEntry> SavedActiveQuests;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	TArray<FName> SavedCompletedQuestIDs;

	// 스토리 진행도 (0 = 게임 시작). 특정 진행도부터 열리는 퀘스트 게이팅에 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	int32 SavedStoryProgress = 0;
	
	//GAS어트리뷰트
	// 저장된 적 없는 구버전 세이브 구분용으로 기본값 -1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|GAS")
	float SavedCurrentHealth = -1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|GAS")
	float SavedCurrentBattery = -1.f;
	
	//인벤토리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	TArray<FGuestSavedInventoryEntry> SavedInventory;
};