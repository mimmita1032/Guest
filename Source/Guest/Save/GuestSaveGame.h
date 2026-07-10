#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Guest/Data/DataTable/GQuestTypes.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
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
	// 현재 진행 단계 ID — DataTable 단계 순서 변경/삽입에도 안전하도록 인덱스 대신 ID 저장
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	FName CurrentStepID = NAME_None;
	// 각 목표 누적 수치 (ObjectiveID로 매칭)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	TArray<FQuestObjectiveProgress> ObjectiveProgress;
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
	static constexpr int32 CurrentSaveVersion = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save|Meta")
	int32 SaveVersion = CurrentSaveVersion;
	
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

	// 세계 시간 (0.0~24.0시). 저장된 적 없는 구버전 세이브 구분용 기본값 -1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Spacetime")
	float SavedWorldHour = -1.f;

	// 디지캠으로 촬영한 사진 (Snapshot 텍스처는 직렬화되지 않고 CompressedImage에서 재생성)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Photo")
	TArray<FPhotoData> SavedPhotos;
};
