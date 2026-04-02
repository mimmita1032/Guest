// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GItemPickup.h"

#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentVisuals.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Items/Instance//GItemInstance.h"


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

	UGItemInstance* NewInstance = NewObject<UGItemInstance>(this);
	NewInstance->SetItemDefinition(ItemDefinition);
	
	G_LOG(TEXT("%s 아이템을 획득했습니다!"), *ItemDefinition->ItemName.ToString());
	G_LOG(TEXT("%s 인스턴스가 생성되었습니다!"), *ItemDefinition->ItemName.ToString());
    
	// TODO: 인벤토리에 NewInstance 전달

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


