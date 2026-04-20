// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GItemFragment.h"
#include "GItemFragmentNarrative.generated.h"




/*=================
아이템 서사
=================*/
UCLASS()
class GUEST_API UGItemFragmentNarrative : public UGItemFragment
{
	GENERATED_BODY()
	
public:
	//아이템이 속한 현실의 연도
	UPROPERTY(EditDefaultsOnly, Category = "Narrative")
	int32 OriginYear = 1995;
	
	// 아이템 사연 텍스트
	UPROPERTY(EditDefaultsOnly, Category = "Narrative", meta = (MultiLine = true))
	FText Description;

	// UI 자세히보기용 3D메쉬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narrative")
	TObjectPtr<UStaticMesh> InspectMesh;
};
