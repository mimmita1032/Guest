// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GItemPickup.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Guest/Items/Fragments/GItemFragmentVisuals.h"
#include "Guest/Items/Fragments/GItemFragmentNarrative.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"
#include "Guest/Utils/GLog.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"
#include "Guest/Items/Instance/GItemInstance.h"


AGItemPickup::AGItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AGItemPickup::Interact_Implementation(AActor* Interactor)
{
	if (!ItemDefinition) return;

	AGuestCharacter* Player = Cast<AGuestCharacter>(Interactor);
	if (!Player) return;

	UGInventoryComponent* InvComp = Player->FindComponentByClass<UGInventoryComponent>();
	if (!InvComp) return;

	const FInventoryItemHandle Handle = InvComp->GrantItem(ItemDefinition);
	if (!Handle.IsValid())
	{
		G_WARN(TEXT("아이템 획득 실패: 인벤토리가 가득 찼습니다. [%s]"), *ItemDefinition->GetName());
		return;
	}

	G_LOG(TEXT("[%s] 아이템을 획득했습니다."), *ItemDefinition->GetName());

	// 퀘스트 서브시스템에 획득 알림 (인벤토리 성공 후에만)
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UGQuestSubsystem* QuestSys = GI->GetSubsystem<UGQuestSubsystem>())
		{
			QuestSys->OnObjectiveUpdated.Broadcast(ItemDefinition->ItemID, Quantity);
		}
	}

	Destroy();
}

FText AGItemPickup::GetInteractText_Implementation() const
{
	return FText::GetEmpty();
}

void AGItemPickup::BeginPlay()
{
	Super::BeginPlay();
	UpdatePickupVisuals();
}

void AGItemPickup::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdatePickupVisuals();
}

void AGItemPickup::UpdatePickupVisuals() const
{
	if (!ItemDefinition || !MeshComp) return;

	if (const UGItemFragmentVisuals* Visuals = ItemDefinition->FindFragmentByClass<UGItemFragmentVisuals>())
	{
		if (!Visuals->ItemMesh.IsNull())
		{
			MeshComp->SetStaticMesh(Visuals->ItemMesh.LoadSynchronous());
		}
	}
}

void AGItemPickup::InitializePickup(const UGItemDefinition* InDefinition, int32 InQuantity)
{
	ItemDefinition = InDefinition;
	Quantity = InQuantity;
	UpdatePickupVisuals();
}