// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GItemPickup.h"

#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentVisuals.h"
#include "Guest/Utils/GLog.h"


AGItemPickup::AGItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
    
    MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AGItemPickup::Interact(AActor* Interactor)
{
	if (!ItemDefinition) return;

	G_LOG(TEXT("%s 아이템을 획득했습니다!"), *ItemDefinition->ItemName.ToString());
    
	// TODO: 인벤토리 작업

	Destroy();
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

	if (const auto* Visuals = ItemDefinition->FindFragmentByClass<UGItemFragmentVisuals>())
	{
		if (!Visuals->ItemMesh.IsNull())
		{
			MeshComp->SetStaticMesh(Visuals->ItemMesh.LoadSynchronous());
		}
	}
}


