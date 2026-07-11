// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GPhotoLibrarySubsystem.generated.h"

// 레벨 전환에도 사진 목록이 유지되도록 GameInstance 스코프에서 관리
UCLASS()
class GUEST_API UGPhotoLibrarySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void AddPhoto(const FPhotoData& Photo);

	UFUNCTION(BlueprintPure, Category = "Photo")
	const TArray<FPhotoData>& GetPhotos() const { return Photos; }

	UFUNCTION(BlueprintPure, Category = "Photo")
	int32 GetPhotoCount() const { return Photos.Num(); }

	// 갤러리 위젯에서 구독 — 실시간 추가 반영용
	UPROPERTY(BlueprintAssignable, Category = "Photo")
	FOnPhotoTaken OnPhotoAdded;

	UFUNCTION(BlueprintCallable, Category = "Photo|Save")
	void ExportPhotoSaveData(TArray<FPhotoData>& OutPhotos) const;

	// Snapshot 텍스처는 세이브에 직렬화되지 않으므로 CompressedImage(PNG)에서 재생성해 복원
	UFUNCTION(BlueprintCallable, Category = "Photo|Save")
	void ImportPhotoSaveData(const TArray<FPhotoData>& InPhotos);

private:
	UPROPERTY()
	TArray<FPhotoData> Photos;
};
