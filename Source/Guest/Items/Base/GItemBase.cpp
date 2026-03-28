// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Items/Base/GItemBase.h"

#include "Guest/Data/DataAssets/GItemData.h"
#include "Guest/Utils/GLog.h"

AGItemBase::AGItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	InteractionText = FText::FromString(TEXT("아이템"));
}

void AGItemBase::BeginPlay()
{
	// Mesh 자동 적용
	if (ItemData && ItemData.Get()->ItemMesh)
	{
		MeshComp->SetStaticMesh(ItemData->ItemMesh);
	}
}

void AGItemBase::Interact(AActor* Interactor)
{
	if (!Interactor)
	{
		G_ERR(TEXT("InterActor 오류"));
		return;
	}
	if (!ItemData)
	{
		G_ERR(TEXT("%s: 아이템 데이터가 할당되지 않았습니다."), *GetName());
		return;
	}
	
	G_LOG(TEXT("%s와(과) 상호작용했습니다. (주체: %s)"), *GetName(), *Interactor->GetName());
}

FText AGItemBase::GetInteractText() const
{
	// [TODO] Text 구성
	return InteractionText;
}