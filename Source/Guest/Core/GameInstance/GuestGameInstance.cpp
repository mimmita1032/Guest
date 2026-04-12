#include "Guest/Core/GameInstance/GuestGameInstance.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

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

void UGuestGameInstance::RequestLoadFromSlot(const FString& SlotName, int32 UserIndex)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		return;
	}
	
	USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
	UGuestSaveGame* SaveObject = Cast<UGuestSaveGame>(Loaded);
	if (!SaveObject) return;
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	const FString CurrentPackage = GuestGetPersistentMapPackageName(World);
	const FString SavedPackage = SaveObject-> MapPackageName;
	
	//구 세이브 데이터 인 경우(map 없는 버전)
	if (SavedPackage.IsEmpty())
	{
		if (APlayerController* PC = UGameplayStatics::CreatePlayer(World, 0))
		{
			if (APawn* Pawn = PC -> GetPawn())
			{
				Pawn -> SetActorLocation(SaveObject -> PlayerWorld.Location);
				Pawn -> SetActorRotation(SaveObject -> PlayerWorld.Rotation);
			}
		}
		return;
	}
	
	//현재 맵과 저장된 맵이 같은 경우
	if (CurrentPackage.Equals(SavedPackage,ESearchCase::IgnoreCase))
	{
		if (APlayerController* PC = UGameplayStatics::CreatePlayer(World, 0))
		{
			if (APawn* Pawn = PC -> GetPawn())
			{
				Pawn -> SetActorLocation(SaveObject -> PlayerWorld.Location);
				Pawn -> SetActorRotation(SaveObject -> PlayerWorld.Rotation);
			}
		}
	}
	
	PendingPlayerWorld = SaveObject -> PlayerWorld;
	bPendingApplyPlayerWorld = true;
	
	const FString ShortMapName = FPackageName::GetShortName(SavedPackage);
	UGameplayStatics::OpenLevel(World, FName(*ShortMapName));
}

void UGuestGameInstance::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
	if (!bPendingApplyPlayerWorld || !LoadedWorld || !LoadedWorld->IsGameWorld())
	{
		return;
	}
	
	FTimerHandle TimerHandle;
	LoadedWorld->GetTimerManager().SetTimer(
		TimerHandle,
		[this, LoadedWorld]()
		{
			if(!bPendingApplyPlayerWorld)
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
		
			Pawn->SetActorLocation(PendingPlayerWorld.Location, false, nullptr, ETeleportType::TeleportPhysics);
			Pawn->SetActorRotation(PendingPlayerWorld.Rotation, ETeleportType::TeleportPhysics);
		
			bPendingApplyPlayerWorld = false;
		},
		0.05f, false);
}
