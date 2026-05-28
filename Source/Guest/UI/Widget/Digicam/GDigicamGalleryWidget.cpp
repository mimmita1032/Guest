// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GDigicamGalleryWidget.h"
#include "GPhotoEntryObject.h"
#include "Components/ListView.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Guest/Components/GCameraComponent.h"
#include "GameFramework/Pawn.h"

void UGDigicamGalleryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 선택 이벤트는 UGPhotoEntryObject::OnSelected 델리게이트로 처리 (RefreshPhotoList에서 구독)
}

void UGDigicamGalleryWidget::OnTabActivated_Implementation()
{
	// 카메라 구독 (최초 1회 — 이미 바인딩돼 있으면 스킵)
	if (UGCameraComponent* CamComp = GetCameraComponent())
	{
		if (!CamComp->OnPhotoTaken.IsAlreadyBound(this, &UGDigicamGalleryWidget::OnPhotoTaken))
		{
			CamComp->OnPhotoTaken.AddDynamic(this, &UGDigicamGalleryWidget::OnPhotoTaken);
		}
	}

	RefreshPhotoList();
}

void UGDigicamGalleryWidget::RefreshPhotoList()
{
	LV_Photos->ClearListItems();

	UGCameraComponent* CamComp = GetCameraComponent();
	if (!CamComp)
	{
		ShowEmptyState();
		return;
	}

	const TArray<FPhotoData>& Photos = CamComp->GetPhotos();
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

	if (Photo.RenderTarget)
	{
		IMG_SelectedPhoto->SetBrushResourceObject(Photo.RenderTarget);
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

UGCameraComponent* UGDigicamGalleryWidget::GetCameraComponent() const
{
	const APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	const APawn* Pawn = PC->GetPawn();
	if (!Pawn) return nullptr;

	return Pawn->FindComponentByClass<UGCameraComponent>();
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
