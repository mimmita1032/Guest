// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GPhotoLibrarySubsystem.generated.h"

class UGInventoryComponent;

/**
 * 디지캠 「찍은 사진」 메뉴가 쓰는 사진 목록 제공자.
 *
 * 사진의 실제 저장소는 인벤토리 하나뿐이다 — 사진은 FGPhotoItemInstanceData를 들고 있는
 * 일반 아이템이며, 이 서브시스템은 인벤토리에서 그것들만 골라 보여주는 뷰 역할만 한다.
 * 따라서 자체 배열도, 별도 세이브도 없다(인벤토리 세이브에 함께 저장된다).
 */
UCLASS()
class GUEST_API UGPhotoLibrarySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// 인벤토리에서 사진 아이템을 모아 촬영 시각 순으로 반환한다.
	// 매 호출마다 인벤토리를 훑으므로 매 프레임 호출하지 말 것.
	UFUNCTION(BlueprintPure, Category = "Photo")
	TArray<FPhotoData> GetPhotos() const;

	UFUNCTION(BlueprintPure, Category = "Photo")
	int32 GetPhotoCount() const;

	// 촬영 직후 카메라가 호출 — 갤러리가 목록 전체를 다시 훑지 않고 한 장만 추가할 수 있게 한다
	void NotifyPhotoTaken(const FPhotoData& Photo);

	// 세이브에서 인벤토리를 복원한 직후 호출할 것.
	// 스냅샷 텍스처는 직렬화되지 않으므로 CompressedImage(PNG)에서 되살린다.
	// 레벨 전환처럼 메모리에서 그대로 넘어온 경우엔 텍스처가 살아 있어 재디코딩을 건너뛴다.
	void RestorePhotoSnapshots(UGInventoryComponent* InventoryComponent);

	// 갤러리 위젯에서 구독 — 실시간 추가 반영용
	UPROPERTY(BlueprintAssignable, Category = "Photo")
	FOnPhotoTaken OnPhotoAdded;
};
