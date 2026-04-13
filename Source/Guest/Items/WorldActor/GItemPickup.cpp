// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GItemPickup.h"

#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentVisuals.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Items/Fragments/GItemFragmentNarrative.h"
#include "Guest/UI/GameplayTags/GuestGameplayTags.h"
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

	if (ItemDefinition)
	{
		//네이티브 게임플레이 태그 검사
		if (ItemDefinition->HasTag(GuestGameplayTags::TAG_Item_Era_1995))
		{
			UE_LOG(LogGSystem, Log, TEXT("1995년 과거의 물건을 획득했습니다."));
		}

		//프래그먼트 데이터 추출
		if (const UGItemFragmentNarrative* NarrativeFrag = ItemDefinition->FindFragmentByClass<UGItemFragmentNarrative>())
		{
			UE_LOG(LogGSystem, Log, TEXT("아이템 출신 연도: %d"), NarrativeFrag->OriginYear);
			UE_LOG(LogGSystem, Log, TEXT("아이템 사연: %s"), *NarrativeFrag->Description.ToString());
		}
	}
	
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


