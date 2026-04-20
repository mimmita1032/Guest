// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GItemPickup.h"

#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Kismet/GameplayStatics.h"
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
		if (const UGItemFragmentNarrative* NarrativeFrag = ItemDefinition->FindFragmentByClass<UGItemFragmentNarrative>())
		{
			UE_LOG(LogGSystem, Log, TEXT("아이템 출신 연도: %d"), NarrativeFrag->OriginYear);
			UE_LOG(LogGSystem, Log, TEXT("아이템 사연: %s"), *NarrativeFrag->Description.ToString());
		}
	}

	// TODO: 인벤토리에 NewInstance 전달

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
	// 퀘스트 서브시스템에 아이템 획득 알림 (ItemID가 퀘스트 목표 TargetID와 일치하면 자동 갱신)
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UGQuestSubsystem* QuestSys = GI->GetSubsystem<UGQuestSubsystem>())
		{
			QuestSys->OnObjectiveUpdated.Broadcast(ItemDefinition->ItemID, 1);
		}
	}

	Destroy();
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

