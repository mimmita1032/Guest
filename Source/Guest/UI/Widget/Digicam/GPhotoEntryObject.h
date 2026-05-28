// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GPhotoEntryObject.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPhotoEntrySelected, const FPhotoData&);

// ListView는 UObject만 아이템으로 받으므로 FPhotoData를 래핑
UCLASS(BlueprintType)
class GUEST_API UGPhotoEntryObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Photo")
	FPhotoData PhotoData;

	// 선택됐을 때 갤러리 위젯이 구독
	FOnPhotoEntrySelected OnSelected;
};
