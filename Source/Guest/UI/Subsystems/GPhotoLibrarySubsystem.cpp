// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GPhotoLibrarySubsystem.h"
#include "ImageUtils.h"
#include "Guest/Utils/GLog.h"

void UGPhotoLibrarySubsystem::AddPhoto(const FPhotoData& Photo)
{
	Photos.Add(Photo);
	OnPhotoAdded.Broadcast(Photo);
}

void UGPhotoLibrarySubsystem::ExportPhotoSaveData(TArray<FPhotoData>& OutPhotos) const
{
	OutPhotos = Photos;
}

void UGPhotoLibrarySubsystem::ImportPhotoSaveData(const TArray<FPhotoData>& InPhotos)
{
	Photos.Reset();
	Photos.Reserve(InPhotos.Num());

	for (const FPhotoData& Saved : InPhotos)
	{
		FPhotoData Photo = Saved;
		Photo.Snapshot = nullptr;

		if (Photo.CompressedImage.Num() > 0)
		{
			Photo.Snapshot = FImageUtils::ImportBufferAsTexture2D(Photo.CompressedImage);
		}

		// 이미지가 손상됐어도 메타데이터(연도/장소/날짜)는 남기고 복원
		if (!Photo.Snapshot)
		{
			G_WARN(TEXT("사진 복원: 이미지 데이터가 없거나 손상됨 — 썸네일 없이 복원 (%d년 %s)"),
				Photo.InGameYear, *Photo.PlaceName.ToString());
		}

		Photos.Add(MoveTemp(Photo));
		OnPhotoAdded.Broadcast(Photos.Last());
	}
}
