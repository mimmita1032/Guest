#include "Guest/Core/GameInstance/GuestGameInstance.h"

#include "AbilitySystemInterface.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/GAS/GuestAttributeSet.h"
#include "Guest/Save/GuestMapPackageUtils.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/Subsystem/GSpacetimeSubsystem.h"
#include "Guest/UI/Subsystems/GPhotoLibrarySubsystem.h"
#include "Guest/Utils/GLog.h"

void UGuestGameInstance::Init()
{
	Super::Init();
	
	PostLoadMapDelegateHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this, &UGuestGameInstance::OnPostLoadMapWithWorld);
}

void UGuestGameInstance::Shutdown()
{
	if (PostLoadMapDelegateHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapDelegateHandle);
		PostLoadMapDelegateHandle.Reset();
	}
	
	Super::Shutdown();
}

static FString GuestGetPersistentMapPackageName(const UWorld* World)
{
	if (!World || !World -> PersistentLevel)
	{
		return {};
	}
	return World -> PersistentLevel -> GetOutermost()->GetName();
}

void UGuestGameInstance::MarkPointPlaced(FName PlacementID)
{
	if (PlacementID.IsNone()) return;
	PlacedPointIDs.Add(PlacementID);
}

bool UGuestGameInstance::IsPointPlaced(FName PlacementID) const
{
	return !PlacementID.IsNone() && PlacedPointIDs.Contains(PlacementID);
}

bool UGuestGameInstance::TryMarkPlayedOnce(FName EventID)
{
	if (EventID.IsNone()) return true;
	if (PlayedOnceIDs.Contains(EventID)) return false;
	PlayedOnceIDs.Add(EventID);
	return true;
}

void UGuestGameInstance::ExportPlacementSaveData(TArray<FName>& OutPlacedIDs) const
{
	OutPlacedIDs = PlacedPointIDs.Array();
}

void UGuestGameInstance::ImportPlacementSaveData(const TArray<FName>& InPlacedIDs)
{
	PlacedPointIDs.Empty();
	for (const FName& ID : InPlacedIDs)
	{
		if (!ID.IsNone()) PlacedPointIDs.Add(ID);
	}
}

void UGuestGameInstance::RequestLoadFromSlot(const FString& SlotName, int32 UserIndex)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		return;
	}

	USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
	UGuestSaveGame* SaveObject = Cast<UGuestSaveGame>(Loaded);
	if (!SaveObject)
	{
		return;
	}

	if (SaveObject->SaveVersion != UGuestSaveGame::CurrentSaveVersion)
	{
		G_WARN(TEXT("세이브 로드 거부: 지원하지 않는 버전입니다. 저장 버전=%d, 현재 버전=%d"),
			SaveObject->SaveVersion, UGuestSaveGame::CurrentSaveVersion);
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	// ── 퀘스트 복원 (GameInstanceSubsystem이라 맵 전환과 무관하게 유지) ──
	if (UGQuestSubsystem* QuestSys = GetSubsystem<UGQuestSubsystem>())
	{
		QuestSys->ImportQuestSaveData(
			SaveObject->SavedActiveQuests,
			SaveObject->SavedCompletedQuestIDs);
		QuestSys->SetStoryProgress(SaveObject->SavedStoryProgress);
	}

	// ── 세계 시간 복원 (GameInstanceSubsystem이라 맵 전환과 무관하게 즉시 적용) ──
	if (UGSpacetimeSubsystem* SpacetimeSys = GetSubsystem<UGSpacetimeSubsystem>())
	{
		SpacetimeSys->ImportTimeSaveData(SaveObject->SavedWorldHour, SaveObject->SavedWorldDay);
		SpacetimeSys->ImportLocationSaveData(SaveObject->SavedLocationYear, SaveObject->SavedLocationAreaCode);
	}

	// 배치 지점 복원 — 액터보다 먼저 채워야 레벨이 열릴 때 BeginPlay에서 읽을 수 있다
	ImportPlacementSaveData(SaveObject->SavedPlacedPointIDs);

	// 사진은 인벤토리 아이템이므로 인벤토리 복원(ImportInventorySaveData)에 포함된다

	const FString CurrentPackage = GuestGetPersistentMapPackageName(World);
	const FString SavedPackage = SaveObject->MapPackageName;

	// 구 세이브 데이터 (맵 패키지 없음)
	if (SavedPackage.IsEmpty())
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				Pawn->SetActorLocation(SaveObject->PlayerWorld.Location);
				Pawn->SetActorRotation(SaveObject->PlayerWorld.Rotation);
				
				// ── GAS 어트리뷰트 복원 ──
				RestoreGASAttributes(Pawn, SaveObject);
				RestoreInventory(Pawn, SaveObject);
			}
		}
		return;
	}

	const FString CurrentNorm = GuestMapPackage::StripPIEFromPackagePath(CurrentPackage);
	const FString SavedNorm = GuestMapPackage::StripPIEFromPackagePath(SavedPackage);
	const bool bSameMap = CurrentNorm.Equals(SavedNorm, ESearchCase::IgnoreCase);

	if (bSameMap)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				Pawn->SetActorLocation(SaveObject->PlayerWorld.Location, false, nullptr, ETeleportType::TeleportPhysics);
				Pawn->SetActorRotation(SaveObject->PlayerWorld.Rotation, ETeleportType::TeleportPhysics);
				
				// ── GAS 어트리뷰트 복원 ──
				RestoreGASAttributes(Pawn, SaveObject);
				RestoreInventory(Pawn, SaveObject);
			}
		}
		return;
	}

	// ── 다른 맵 로드: 위치/GAS 복원은 OnPostLoadMapWithWorld에서 처리 ──
	PendingPlayerWorld = SaveObject->PlayerWorld;
	PendingSaveObject = SaveObject;  // GAS 복원용으로 SaveObject 보관
	bPendingApplyPlayerWorld = true;

	const FString ShortMapName = FPackageName::GetShortName(SavedNorm);
	UGameplayStatics::OpenLevel(World, FName(*ShortMapName));
}

void UGuestGameInstance::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
	if ((!bPendingApplyPlayerWorld && !PendingSaveObject) || !LoadedWorld || !LoadedWorld->IsGameWorld())
	{
		return;
	}

	FTimerHandle TimerHandle;
	LoadedWorld->GetTimerManager().SetTimer(
		TimerHandle,
		[this, LoadedWorld]()
		{
			if (!bPendingApplyPlayerWorld && !PendingSaveObject)
			{
				return;
			}

			APlayerController* PC = UGameplayStatics::GetPlayerController(LoadedWorld, 0);
			if (!PC)
			{
				return;
			}

			APawn* Pawn = PC ->GetPawn();
			if (!Pawn)
			{
				return;
			}

			// 세이브 로드 시에만 위치 복원 — 시공간 이동(CarryPlayerStateAcrossTravel)은 새 레벨의 기본 스폰 위치를 그대로 씀
			if (bPendingApplyPlayerWorld)
			{
				Pawn->SetActorLocation(PendingPlayerWorld.Location, false, nullptr, ETeleportType::TeleportPhysics);
				Pawn->SetActorRotation(PendingPlayerWorld.Rotation, ETeleportType::TeleportPhysics);
				bPendingApplyPlayerWorld = false;
			}

			// ── 맵 전환 후 GAS 어트리뷰트/인벤토리 복원 ──
			if (PendingSaveObject)
			{
				RestoreGASAttributes(Pawn, PendingSaveObject);
				RestoreInventory(Pawn, PendingSaveObject);
				PendingSaveObject = nullptr;
			}
		},
		0.05f, false);
}

void UGuestGameInstance::CarryPlayerStateAcrossTravel()
{
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	if (!Pawn) return;

	UGuestSaveGame* Snapshot = NewObject<UGuestSaveGame>(this);

	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn))
	{
		if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
		{
			Snapshot->SavedCurrentHealth  = ASC->GetNumericAttribute(UGuestAttributeSet::GetCurrentHealthAttribute());
			Snapshot->SavedCurrentBattery = ASC->GetNumericAttribute(UGuestAttributeSet::GetCurrentBatteryAttribute());
		}
	}

	if (UGInventoryComponent* Inv = Pawn->FindComponentByClass<UGInventoryComponent>())
	{
		Inv->ExportInventorySaveData(Snapshot->SavedInventory);
	}

	PendingSaveObject = Snapshot;
}

void UGuestGameInstance::RestoreInventory(APawn* Pawn, const UGuestSaveGame* SaveObject)
{
	if (!Pawn || !SaveObject) return;

	UGInventoryComponent* Inv = Pawn->FindComponentByClass<UGInventoryComponent>();
	if (!Inv) return;

	Inv->ImportInventorySaveData(SaveObject->SavedInventory);

	// 사진은 인벤토리 아이템이므로 복원 직후 스냅샷 텍스처를 되살려야 갤러리에 보인다
	if (UGPhotoLibrarySubsystem* PhotoLib = GetSubsystem<UGPhotoLibrarySubsystem>())
	{
		PhotoLib->RestorePhotoSnapshots(Inv);
	}
}

void UGuestGameInstance::RestoreGASAttributes(APawn* Pawn, const UGuestSaveGame* SaveObject)
{
	if (!Pawn || !SaveObject) return;
	if (SaveObject->SavedCurrentHealth < 0.f) return;
	
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn);
	if (!ASCInterface) return;
	
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (!ASC) return;
	
	ASC->SetNumericAttributeBase(
		UGuestAttributeSet::GetCurrentHealthAttribute(),
		SaveObject->SavedCurrentHealth);
	
	ASC->SetNumericAttributeBase(
		UGuestAttributeSet::GetCurrentBatteryAttribute(),
		SaveObject->SavedCurrentBattery);
}
