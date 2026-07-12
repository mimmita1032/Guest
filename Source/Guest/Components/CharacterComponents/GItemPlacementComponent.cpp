// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GItemPlacementComponent.h"
#include "GInventoryComponent.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/WorldActor/GItemPlacementGhost.h"
#include "Guest/Items/WorldActor/GItemPickup.h"
#include "Guest/Utils/GLog.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

UGItemPlacementComponent::UGItemPlacementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false);
}

void UGItemPlacementComponent::BeginPlacement(const UGItemDefinition* ItemDef, FInventoryItemHandle SourceHandle)
{
	if (!ItemDef || !SourceHandle.IsValid() || bIsPlacementActive) return;

	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	if (!World || !Owner) return;

	PendingItemDef = ItemDef;
	PendingSourceHandle = SourceHandle;

	const TSubclassOf<AGItemPlacementGhost> SpawnClass = GhostClass ? GhostClass.Get() : AGItemPlacementGhost::StaticClass();
	GhostActor = World->SpawnActor<AGItemPlacementGhost>(SpawnClass, Owner->GetActorTransform());
	if (GhostActor)
	{
		GhostActor->SetItemDefinition(ItemDef);
	}

	bIsPlacementActive = true;
	bIsValidPlacement = false;
	SetComponentTickEnabled(true);

	G_LOG(TEXT("아이템 배치 모드 시작: [%s]"), *ItemDef->ItemID.ToString());
	OnPlacementStateChanged.Broadcast(true);
}

void UGItemPlacementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateGhostTransform();
}

void UGItemPlacementComponent::UpdateGhostTransform()
{
	if (!bIsPlacementActive || !GhostActor) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	APlayerController* PC = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;
	if (!PC) return;

	FVector WorldLocation, WorldDirection;
	if (!PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection)) return;

	const FVector TraceEnd = WorldLocation + WorldDirection * TraceDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ItemPlacementTrace), false);
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(GhostActor);

	FHitResult Hit;
	bIsValidPlacement = GetWorld()->LineTraceSingleByChannel(Hit, WorldLocation, TraceEnd, TraceChannel, Params);

	FVector TargetLocation = bIsValidPlacement ? Hit.Location : TraceEnd;

	// 캐릭터 기준 MaxPlacementRange를 벗어나면 그 방향으로 최대거리까지만 당겨오고 배치 불가 처리 —
	// 배치 모드 중에도 캐릭터가 이동 가능해서, 너무 먼 곳에 마우스를 갖다 대는 것만으로 배치되는 걸 막음
	const FVector OwnerLocation = OwnerPawn->GetActorLocation();
	const FVector Offset = TargetLocation - OwnerLocation;
	const float Distance = Offset.Size();
	if (Distance > MaxPlacementRange)
	{
		TargetLocation = OwnerLocation + Offset.GetSafeNormal() * MaxPlacementRange;
		bIsValidPlacement = false;
	}

	GhostActor->SetActorLocation(TargetLocation);
	GhostActor->SetValidState(bIsValidPlacement);
}

void UGItemPlacementComponent::ConfirmPlacement()
{
	if (!bIsPlacementActive) return;

	if (!bIsValidPlacement || !PendingItemDef || !GhostActor)
	{
		G_WARN(TEXT("아이템 배치 확정 실패: 현재 고스트 위치가 유효하지 않습니다(트레이스가 바닥/벽을 못 찾음)."));
		CancelPlacement();
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	UGInventoryComponent* Inv = OwnerPawn ? OwnerPawn->FindComponentByClass<UGInventoryComponent>() : nullptr;
	if (!Inv || !Inv->RemoveItem(PendingSourceHandle))
	{
		G_WARN(TEXT("아이템 배치 확정 실패: 인벤토리에서 제거할 수 없습니다."));
		CancelPlacement();
		return;
	}

	UWorld* World = GetWorld();
	const FTransform SpawnTransform(GhostActor->GetActorRotation(), GhostActor->GetActorLocation());

	AGItemPickup* Pickup = World->SpawnActorDeferred<AGItemPickup>(
		AGItemPickup::StaticClass(),
		SpawnTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (Pickup)
	{
		Pickup->InitializePickup(PendingItemDef, 1);
		Pickup->FinishSpawning(SpawnTransform);
	}

	G_LOG(TEXT("아이템 배치 확정: [%s] at %s"), *PendingItemDef->ItemID.ToString(), *SpawnTransform.GetLocation().ToString());
	EndPlacement();
}

void UGItemPlacementComponent::CancelPlacement()
{
	if (!bIsPlacementActive) return;

	G_LOG(TEXT("아이템 배치 취소"));
	EndPlacement();
}

void UGItemPlacementComponent::EndPlacement()
{
	if (GhostActor)
	{
		GhostActor->Destroy();
		GhostActor = nullptr;
	}

	PendingItemDef = nullptr;
	PendingSourceHandle = FInventoryItemHandle();
	bIsPlacementActive = false;
	bIsValidPlacement = false;
	SetComponentTickEnabled(false);

	OnPlacementStateChanged.Broadcast(false);
}
