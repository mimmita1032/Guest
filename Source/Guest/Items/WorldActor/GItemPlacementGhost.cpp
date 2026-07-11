// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GItemPlacementGhost.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentVisuals.h"

AGItemPlacementGhost::AGItemPlacementGhost()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorEnableCollision(false);
}

void AGItemPlacementGhost::BeginPlay()
{
	Super::BeginPlay();
	ApplyGhostMaterial();
}

void AGItemPlacementGhost::SetItemDefinition(const UGItemDefinition* ItemDef)
{
	if (!ItemDef || !MeshComp) return;

	if (const UGItemFragmentVisuals* Visuals = ItemDef->FindFragmentByClass<UGItemFragmentVisuals>())
	{
		if (!Visuals->ItemMesh.IsNull())
		{
			MeshComp->SetStaticMesh(Visuals->ItemMesh.LoadSynchronous());
		}
	}

	ApplyGhostMaterial();
}

void AGItemPlacementGhost::ApplyGhostMaterial()
{
	if (!MeshComp || !GhostMaterial) return;

	const int32 NumMaterials = MeshComp->GetNumMaterials();
	if (NumMaterials <= 0) return;

	DynamicMaterial = UMaterialInstanceDynamic::Create(GhostMaterial, this);
	for (int32 Index = 0; Index < NumMaterials; ++Index)
	{
		MeshComp->SetMaterial(Index, DynamicMaterial);
	}
}

void AGItemPlacementGhost::SetValidState(bool bValid)
{
	if (!DynamicMaterial) return;

	static const FName TintParamName(TEXT("Tint"));
	DynamicMaterial->SetVectorParameterValue(TintParamName, bValid ? FLinearColor::Green : FLinearColor::Red);
}
