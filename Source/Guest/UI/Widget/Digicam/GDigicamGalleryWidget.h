// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GDigicamTabBase.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GDigicamGalleryWidget.generated.h"

class UListView;
class UImage;
class UTextBlock;
class UWidget;
class UGPhotoLibrarySubsystem;

// 디지캠 갤러리 탭 — 촬영된 사진 목록과 상세 정보 표시
UCLASS(Abstract)
class GUEST_API UGDigicamGalleryWidget : public UGDigicamTabBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	// 탭 진입 시 목록 갱신 + 카메라 구독
	virtual void OnTabActivated_Implementation() override;

protected:
	// 사진 목록 (WBP_DigiTab_Gallery에서 BindWidget 이름 맞출 것)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UListView> LV_Photos;

	// 선택된 사진 상세 패널
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> IMG_SelectedPhoto;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_SelectedYear;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_SelectedPlaceName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_SelectedDate;

	/** 장소명 줄바꿈 기준 폭(px). 0이면 부모 슬롯이 준 폭에서 접는다. */
	UPROPERTY(EditAnywhere, Category = "Digicam|Layout", meta = (ClampMin = "0.0"))
	float PlaceNameWrapTextAt = 0.f;

	// 사진 0장일 때 표시할 안내 위젯 (선택적)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UWidget> WGT_EmptyHint;

private:
	void RefreshPhotoList();
	void ShowPhotoDetail(const FPhotoData& Photo);
	void ShowEmptyState();
	UGPhotoLibrarySubsystem* GetPhotoLibrary() const;

	UFUNCTION()
	void OnPhotoTaken(const FPhotoData& Photo);

	// UGPhotoEntryObject::OnSelected 델리게이트 콜백
	void OnPhotoEntrySelected(const FPhotoData& Photo);
};
