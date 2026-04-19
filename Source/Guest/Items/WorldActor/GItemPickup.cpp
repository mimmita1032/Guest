// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GItemPickup.h"

#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentVisuals.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Items/Fragments/GItemFragmentNarrative.h"
#include "Guest/UI/GameplayTags/GuestGameplayTags.h"
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
	UGItemInstance* InstanceToGive = ItemInstance.Get();
	
	if (!InstanceToGive)
	{
		if (!ItemDefinition) return;
		InstanceToGive = NewObject<UGItemInstance>(this);
		InstanceToGive->SetItemDefinition(ItemDefinition);
		G_LOG(TEXT("%s 인스턴스가 새로 생성되었습니다!"), *ItemDefinition->ItemName.ToString());
	}
	else
	{
		G_LOG(TEXT("기존 데이터(인스턴스)를 간직한 아이템을 다시 줍습니다."));
	}

	if (AGuestCharacter* Player = Cast<AGuestCharacter>(Interactor))
	{
		if (UGInventoryComponent* InvComp = Player->FindComponentByClass<UGInventoryComponent>())
		{
			if (InvComp->AutoAddItem(InstanceToGive))
			{
				Destroy();
				return;
			}
		}
	}

	G_WARN(TEXT("아이템 획득 실패: 가방이 가득 찼거나 인벤토리를 찾을 수 없습니다."));
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

	if (const auto* Visuals = ItemDefinition->FindFragmentByClass<UGItemFragmentVisuals>())
	{
		if (!Visuals->ItemMesh.IsNull())
		{
			MeshComp->SetStaticMesh(Visuals->ItemMesh.LoadSynchronous());
		}
	}
}

void AGItemPickup::InitializePickup(UGItemInstance* InInstance)
{
	if (InInstance)
	{
		ItemInstance = InInstance;
		
		ItemDefinition = InInstance->ItemDef; 
		
		UpdatePickupVisuals(); 
	}
}

