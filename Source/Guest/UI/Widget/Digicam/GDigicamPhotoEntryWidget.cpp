// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GDigicamPhotoEntryWidget.h"
#include "GPhotoEntryObject.h"
#include "Guest/Utils/GuestBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UGDigicamPhotoEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	CachedEntryObj = Cast<UGPhotoEntryObject>(ListItemObject);
	if (!CachedEntryObj.IsValid()) return;

	const FPhotoData& Photo = CachedEntryObj->PhotoData;

	if (Photo.Snapshot)
	{
		IMG_Thumbnail->SetBrushFromTexture(Photo.Snapshot);
	}

	TXT_Year->SetText(FText::AsNumber(Photo.InGameYear));

	// 장소명은 길이가 제각각이라 목록 칸을 넘긴다 — 잘리는 대신 접히게 한다.
	UGuestBlueprintLibrary::ApplyAutoWrap(TXT_PlaceName, PlaceNameWrapTextAt);
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
