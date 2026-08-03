// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/Items/Instance/GItemInstanceData.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GPhotoItemInstanceData.generated.h"

/*========================
사진 아이템의 개체별 데이터.

사진은 설계도(DA_Item_Photo) 하나를 모든 사진이 공유하고,
실제 내용 — 이미지, 촬영 장소, 시간 — 은 개체마다 여기에 들어간다.

DSLR 촬영 설정(노출 보정 / 조리개 / 셔터스피드 / ISO / 초점)은
추후 FPhotoData에 필드로 추가하면 된다. FInstancedStruct 직렬화는
없던 필드를 기본값으로 채워 로드하므로 세이브 마이그레이션이 필요 없다.
========================*/
USTRUCT(BlueprintType)
struct GUEST_API FGPhotoItemInstanceData : public FGItemInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Photo")
	FPhotoData PhotoData;

	// 인벤토리에서 사진마다 실제로 찍힌 장면이 보이도록 촬영 스냅샷을 아이콘으로 쓴다.
	// 스냅샷이 없으면(이미지 손상 등) nullptr을 반환해 설계도 기본 아이콘으로 대체된다.
	virtual UTexture2D* GetRuntimeIcon() const override { return PhotoData.Snapshot; }
};
