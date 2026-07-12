// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GItemPlacementGhost.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
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

	// Screen Space라 3D 회전(빌보드) 처리 없이도 항상 카메라를 보고 있는 것처럼 렌더링됨 —
	// UMG 위젯이라 프로젝트 기본 폰트를 그대로 써서 한글도 문제없이 표시됨
	PromptWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("PromptWidgetComp"));
	PromptWidgetComp->SetupAttachment(RootComponent);
	PromptWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
	PromptWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	PromptWidgetComp->SetDrawSize(FVector2D(200.f, 50.f));
}

void AGItemPlacementGhost::BeginPlay()
{
	Super::BeginPlay();
	ApplyGhostMaterial();

	if (PromptWidgetComp && PromptWidgetClass)
	{
		PromptWidgetComp->SetWidgetClass(PromptWidgetClass);
	}
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
