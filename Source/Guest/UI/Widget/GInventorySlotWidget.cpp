// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventorySlotWidget.h"
#include "Components/Image.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
#include "Guest/Utils/GLog.h"

void UGInventorySlotWidget::SetItemReference(UGItemInstance* NewItem)
{
	ItemReference = NewItem;

	if (!Img_Icon) return;

	if (ItemReference)
	{
		if (const UGItemDefinition* ItemData = ItemReference->ItemDef) 
		{
			if (const UGItemFragmentInventory* InvFrag = ItemData->FindFragmentByClass<UGItemFragmentInventory>())
			{
				if (InvFrag->ItemIcon.IsValid())
				{
					Img_Icon->SetBrushFromTexture(InvFrag->ItemIcon.LoadSynchronous());
					Img_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					return;
				}
			}
		}
	}

	Img_Icon->SetVisibility(ESlateVisibility::Hidden);
}