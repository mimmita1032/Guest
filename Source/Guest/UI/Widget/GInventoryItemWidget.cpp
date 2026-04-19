// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventoryItemWidget.h"
#include "Components/Image.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"

void UGInventoryItemWidget::SetItemData(UGItemInstance* InItem)
{
	ItemReference = InItem;
	if (!Img_ItemIcon || !ItemReference) return;

	if (const UGItemDefinition* ItemData = ItemReference->ItemDef)
	{
		if (const UGItemFragmentInventory* InvFrag = ItemData->FindFragmentByClass<UGItemFragmentInventory>())
		{
			if (UTexture2D* IconTexture = InvFrag->ItemIcon.LoadSynchronous())
			{
				Img_ItemIcon->SetBrushFromTexture(IconTexture);
			}
		}
	}
}