// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestCommonTabButtonBase.h"
#include "CommonTextBlock.h"

void UGuestCommonTabButtonBase::SetTabName(const FText& InTabName)
{
	if (Text_TabName)
	{
		Text_TabName->SetText(InTabName);
	}
}