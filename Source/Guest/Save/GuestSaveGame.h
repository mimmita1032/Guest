#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Guest/Data/DataTable/GQuestTypes.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "StructUtils/InstancedStruct.h"
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
struct FGuestSavedQuestObjective
{
	GENERATED_BODY()

	// 현재 단계 안에서 목표를 찾는 저장용 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	FName ObjectiveID = NAME_None;

	// 해당 목표의 누적 진행 수치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Quest")
	int32 Count = 0;
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
	TArray<FGuestSavedQuestObjective> ObjectiveProgress;
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

	// 개체별 데이터 (FGItemInstanceData 파생). 설계도만으로 설명되는 아이템은 비어 있다.
	// 구버전 세이브에는 이 필드가 없으므로 빈 값으로 로드되며, 그대로 정상 동작한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	FInstancedStruct InstanceData;
};

UCLASS()
class GUEST_API UGuestSaveGame : public USaveGame
{
	GENERATED_BODY()
	
	public:
	static constexpr int32 CurrentSaveVersion = 6;

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

	// 배치가 끝난 지점 ID. 비어 있어도 그대로 동작하므로 구버전 세이브와 호환된다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|World")
	TArray<FName> SavedPlacedPointIDs;

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

	// 세계 날짜 (1일차부터). 날짜 개념이 없던 구버전 세이브 구분용 기본값 -1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Spacetime")
	int32 SavedWorldDay = -1;

	// 저장 시점의 시공간 좌표 (연도 + 구역). 좌표 자체가 아니라 좌표를 찾는 열쇠만 저장한다 —
	// DT_SpacetimeData의 다른 필드가 바뀌어도 로드 시 최신 값을 따라간다.
	// 저장된 적 없는 구버전 세이브 구분용 기본값 -1 (그 경우 레벨 이름으로 역조회)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Spacetime")
	int32 SavedLocationYear = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Spacetime")
	int32 SavedLocationAreaCode = -1;

	// 디지캠으로 촬영한 사진은 인벤토리 아이템(FGPhotoItemInstanceData)이므로
	// SavedInventory에 함께 저장된다 — 별도 배열을 두지 않는다
};
