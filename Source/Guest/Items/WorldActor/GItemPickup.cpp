// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GItemPickup.h"

#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
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

void AGItemPickup::Interact_Implementation(AActor* Interactor)
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

	// 인벤토리 컴포넌트를 찾아 자동 추가(AutoAddItem)
	if (AGuestCharacter* Player = Cast<AGuestCharacter>(Interactor))
	{
		if (UGInventoryComponent* InvComp = Player->FindComponentByClass<UGInventoryComponent>())
		{
			// 빈 공간을 찾아 성공적으로 넣었다면(true), 월드에서 아이템 액터 삭제
			if (InvComp->AutoAddItem(NewInstance))
			{
				Destroy();
				return;
			}
		}
	}

	// 인벤토리가 없거나 가방이 꽉 차서 false면 파괴하지 않음
	G_WARN(TEXT("아이템 획득 실패: 가방이 가득 찼거나 인벤토리를 찾을 수 없습니다."));
	
}

FText AGItemPickup::GetInteractText_Implementation() const
{
	// 블루프린트에서 오버라이드하지 않았을 때 출력될 기본값
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


