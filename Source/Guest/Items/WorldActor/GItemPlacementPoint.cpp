// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GItemPlacementPoint.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Guest/Characters/Player/GuestCharacter.h"
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

	if (const UGItemFragmentVisuals* Visuals = RequiredItem->FindFragmentByClass<UGItemFragmentVisuals>())
	{
		if (!Visuals->ItemMesh.IsNull())
		{
			PlacedMeshComp->SetStaticMesh(Visuals->ItemMesh.LoadSynchronous());
		}
	}
	PlacedMeshComp->SetVisibility(true);

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
