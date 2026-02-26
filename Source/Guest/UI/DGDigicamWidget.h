// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "DGDigicamWidget.generated.h"

UCLASS()
class GUEST_API UDGDigicamWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 디카 데이터 갱신용 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Digicam")
	void UpdateLCD(EDigicamState CurrentState, int32 Year, int32 AreaCode, FSpacetimeData MatchedData);
};