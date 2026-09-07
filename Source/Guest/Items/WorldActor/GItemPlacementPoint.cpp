// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GItemPlacementPoint.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Core/GameInstance/GuestGameInstance.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentVisuals.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/Utils/GLog.h"
#include "Kismet/GameplayStatics.h"

AGItemPlacementPoint::AGItemPlacementPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	RootComponent = InteractionVolume;
	InteractionVolume->SetBoxExtent(FVector(30.f));
	InteractionVolume->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	PlacedMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlacedMeshComp"));
	PlacedMeshComp->SetupAttachment(RootComponent);
	PlacedMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlacedMeshComp->SetVisibility(false);
}

void AGItemPlacementPoint::BeginPlay()
{
	Super::BeginPlay();

	// 이 지점에 이미 놓았는지는 GameInstance가 들고 있다.
	// 액터는 맵을 다시 열 때마다 새로 만들어지므로 스스로 기억하지 못한다.
	if (const UGuestGameInstance* GuestGI = Cast<UGuestGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if (GuestGI->IsPointPlaced(ResolvePlacementID()))
		{
			bIsPlaced = true;
			ApplyPlacedVisuals();
			return;
		}
	}

	UpdatePreviewVisuals();
}

FName AGItemPlacementPoint::ResolvePlacementID() const
{
	return PlacementID.IsNone() ? GetFName() : PlacementID;
}

void AGItemPlacementPoint::ApplyPlacedVisuals()
{
	if (!RequiredItem || !PlacedMeshComp) return;

	if (const UGItemFragmentVisuals* Visuals = RequiredItem->FindFragmentByClass<UGItemFragmentVisuals>())
	{
		if (!Visuals->ItemMesh.IsNull())
		{
			PlacedMeshComp->SetStaticMesh(Visuals->ItemMesh.LoadSynchronous());
		}
	}

	// SetMaterial(nullptr) = 에셋 기본값으로 복귀. 고스트가 남으면 검게 보인다.
	const int32 NumMaterials = PlacedMeshComp->GetNumMaterials();
	for (int32 Index = 0; Index < NumMaterials; ++Index)
	{
		PlacedMeshComp->SetMaterial(Index, nullptr);
	}
	DynamicGhostMaterial = nullptr;

	PlacedMeshComp->SetVisibility(true);
}

void AGItemPlacementPoint::UpdatePreviewVisuals()
{
	if (bIsPlaced || !RequiredItem || !PlacedMeshComp) return;

	if (const UGItemFragmentVisuals* Visuals = RequiredItem->FindFragmentByClass<UGItemFragmentVisuals>())
	{
		if (!Visuals->ItemMesh.IsNull())
		{
			PlacedMeshComp->SetStaticMesh(Visuals->ItemMesh.LoadSynchronous());
		}
	}

	if (GhostMaterial)
	{
		const int32 NumMaterials = PlacedMeshComp->GetNumMaterials();
		DynamicGhostMaterial = UMaterialInstanceDynamic::Create(GhostMaterial, this);
		for (int32 Index = 0; Index < NumMaterials; ++Index)
		{
			PlacedMeshComp->SetMaterial(Index, DynamicGhostMaterial);
		}
	}

	PlacedMeshComp->SetVisibility(true);
}

void AGItemPlacementPoint::Interact_Implementation(AActor* Interactor)
{
	if (bIsPlaced || !RequiredItem) return;

	AGuestCharacter* Player = Cast<AGuestCharacter>(Interactor);
	if (!Player) return;

	UGInventoryComponent* InvComp = Player->FindComponentByClass<UGInventoryComponent>();
	if (!InvComp) return;

	const FInventoryItemHandle Handle = InvComp->FindHandleByItemID(RequiredItem->ItemID);
	if (!Handle.IsValid())
	{
		G_LOG(TEXT("아이템 배치 실패: 인벤토리에 [%s]가 없습니다."), *RequiredItem->ItemID.ToString());
		return;
	}

	if (!InvComp->RemoveItem(Handle)) return;

	bIsPlaced = true;

	ApplyPlacedVisuals();

	// 맵을 다시 열어도 놓인 상태가 유지되도록 기록한다
	if (UGuestGameInstance* GuestGI = Cast<UGuestGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GuestGI->MarkPointPlaced(ResolvePlacementID());
	}

	G_LOG(TEXT("[%s]를 배치했습니다."), *RequiredItem->ItemID.ToString());

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UGQuestSubsystem* QuestSys = GI->GetSubsystem<UGQuestSubsystem>())
		{
			QuestSys->OnObjectiveUpdated.Broadcast(RequiredItem->ItemID, 1);
		}
	}
}

FText AGItemPlacementPoint::GetInteractText_Implementation() const
{
	return FText::GetEmpty();
}
