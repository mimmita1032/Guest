// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GDigicamGalleryWidget.h"
#include "GPhotoEntryObject.h"
#include "Components/ListView.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Guest/UI/Subsystems/GPhotoLibrarySubsystem.h"

void UGDigicamGalleryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 서브시스템 구독 (최초 1회)
	if (UGPhotoLibrarySubsystem* PhotoLib = GetPhotoLibrary())
	{
		PhotoLib->OnPhotoAdded.AddDynamic(this, &UGDigicamGalleryWidget::OnPhotoTaken);
	}
}

void UGDigicamGalleryWidget::OnTabActivated_Implementation()
{
	RefreshPhotoList();
}

void UGDigicamGalleryWidget::RefreshPhotoList()
{
	LV_Photos->ClearListItems();

	UGPhotoLibrarySubsystem* PhotoLib = GetPhotoLibrary();
	if (!PhotoLib)
	{
		ShowEmptyState();
		return;
	}

	const TArray<FPhotoData>& Photos = PhotoLib->GetPhotos();
	if (Photos.IsEmpty())
	{
		ShowEmptyState();
		return;
	}

	for (const FPhotoData& Photo : Photos)
	{
		UGPhotoEntryObject* EntryObj = NewObject<UGPhotoEntryObject>(this);
		EntryObj->PhotoData = Photo;
		EntryObj->OnSelected.AddUObject(this, &UGDigicamGalleryWidget::OnPhotoEntrySelected);
		LV_Photos->AddItem(EntryObj);
	}

	// 가장 최근 사진 선택
	LV_Photos->SetSelectedItem(LV_Photos->GetItemAt(Photos.Num() - 1));
}

void UGDigicamGalleryWidget::ShowPhotoDetail(const FPhotoData& Photo)
{
	if (WGT_EmptyHint)
	{
		WGT_EmptyHint->SetVisibility(ESlateVisibility::Collapsed);
	}

	IMG_SelectedPhoto->SetVisibility(ESlateVisibility::HitTestInvisible);
	TXT_SelectedYear->SetVisibility(ESlateVisibility::HitTestInvisible);
	TXT_SelectedPlaceName->SetVisibility(ESlateVisibility::HitTestInvisible);
	TXT_SelectedDate->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (Photo.Snapshot)
	{
		IMG_SelectedPhoto->SetBrushFromTexture(Photo.Snapshot);
	}

	TXT_SelectedYear->SetText(FText::AsNumber(Photo.InGameYear));
	TXT_SelectedPlaceName->SetText(Photo.PlaceName);
	TXT_SelectedDate->SetText(Photo.StoryDate);
}

void UGDigicamGalleryWidget::ShowEmptyState()
{
	IMG_SelectedPhoto->SetVisibility(ESlateVisibility::Collapsed);
	TXT_SelectedYear->SetVisibility(ESlateVisibility::Collapsed);
	TXT_SelectedPlaceName->SetVisibility(ESlateVisibility::Collapsed);
	TXT_SelectedDate->SetVisibility(ESlateVisibility::Collapsed);

	if (WGT_EmptyHint)
	{
		WGT_EmptyHint->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

UGPhotoLibrarySubsystem* UGDigicamGalleryWidget::GetPhotoLibrary() const
{
	const UGameInstance* GI = GetGameInstance();
	if (!GI) return nullptr;

	return GI->GetSubsystem<UGPhotoLibrarySubsystem>();
}

void UGDigicamGalleryWidget::OnPhotoTaken(const FPhotoData& Photo)
{
	UGPhotoEntryObject* EntryObj = NewObject<UGPhotoEntryObject>(this);
	EntryObj->PhotoData = Photo;
	EntryObj->OnSelected.AddUObject(this, &UGDigicamGalleryWidget::OnPhotoEntrySelected);
	LV_Photos->AddItem(EntryObj);
	LV_Photos->SetSelectedItem(EntryObj);
}

void UGDigicamGalleryWidget::OnPhotoEntrySelected(const FPhotoData& Photo)
{
	ShowPhotoDetail(Photo);
}
