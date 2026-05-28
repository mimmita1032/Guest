// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GDigicamPhotoEntryWidget.h"
#include "GPhotoEntryObject.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UGDigicamPhotoEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	CachedEntryObj = Cast<UGPhotoEntryObject>(ListItemObject);
	if (!CachedEntryObj.IsValid()) return;

	const FPhotoData& Photo = CachedEntryObj->PhotoData;

	if (Photo.RenderTarget)
	{
		IMG_Thumbnail->SetBrushResourceObject(Photo.RenderTarget);
	}

	TXT_Year->SetText(FText::AsNumber(Photo.InGameYear));
	TXT_PlaceName->SetText(Photo.PlaceName);
}

void UGDigicamPhotoEntryWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserListEntry::NativeOnItemSelectionChanged(bIsSelected);

	if (bIsSelected && CachedEntryObj.IsValid())
	{
		CachedEntryObj->OnSelected.Broadcast(CachedEntryObj->PhotoData);
	}
}
