// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Items/Base/GItemBase.h"
#include "Guest/Utils/GLog.h"

AGItemBase::AGItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	InteractionText = FText::FromString(TEXT("아이템"));
}

void AGItemBase::Interact(AActor* Interactor)
{
	G_LOG(TEXT("%s와(과) 상호작용했습니다. (주체: %s)"), *GetName(), *Interactor->GetName());
}

FText AGItemBase::GetInteractText() const
{
	return InteractionText;
}