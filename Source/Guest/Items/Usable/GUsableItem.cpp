// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GUsableItem.h"
#include "Guest/Utils/GLog.h"

AGUsableItem::AGUsableItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGUsableItem::Interact(AActor* Interactor)
{
	Super::Interact(Interactor);
	
	Use(Interactor);
}

void AGUsableItem::Use(AActor* User)
{
	if (!User) return;
	
	G_LOG(TEXT("Usable 아이템 [%s]이(가) 사용되었습니다. (주체: %s"),*GetName(), *User->GetName());
	if (OnUseAction.IsBound())
	{
		OnUseAction.Execute(User);
	}
	else
	{
		G_ERR(TEXT("실행할 UsageAction이 바인딩되지 않았습니다."));
	}
}
