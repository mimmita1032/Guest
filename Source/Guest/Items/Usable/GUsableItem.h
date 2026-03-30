// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/Items/Base/GItemBase.h"
#include "GUsableItem.generated.h"

// 아이템 사용시 실행될 함수
DECLARE_DELEGATE_OneParam(FOnUseAction, AActor* User);

UCLASS()
class GUEST_API AGUsableItem : public AGItemBase
{
	GENERATED_BODY()

public:
	AGUsableItem();
	
	// Interact 구현을 통한 아이템 사용
	virtual void Interact(AActor* Interactor) override;
	
	// 실제 아이템 사용 로직
	virtual void Use(AActor* User);
	
	// 실행할 함수를 설정하는 함수
	void BindUsageAction(FOnUseAction InAction) { OnUseAction = InAction;}
	
protected:
	// 전달된 함수 보관 변수
	FOnUseAction OnUseAction;
};
