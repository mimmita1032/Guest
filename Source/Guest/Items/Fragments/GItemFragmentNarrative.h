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
	int32 OriginYear;
	
	// 플레이어가 아이템을 관찰할 때 읽을 수 있는 사연
	UPROPERTY(EditDefaultsOnly, Category = "Narrative", meta = (MultiLine = true))
	FText Description;
};
