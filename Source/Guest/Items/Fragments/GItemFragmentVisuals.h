// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GItemFragment.h"
#include "GItemFragmentVisuals.generated.h"

/*=================
레벨에서 보이는 3D 외형
=================*/
UCLASS()
class GUEST_API UGItemFragmentVisuals : public UGItemFragment
{
	GENERATED_BODY()
	
public:
	//3D Mesh
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSoftObjectPtr<UStaticMesh> ItemMesh;
};
