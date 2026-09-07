// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GObjectiveEntryWidget.generated.h"

class UCheckBox;
class UTextBlock;

UCLASS()
class GUEST_API UGObjectiveEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(const FString& InObjectiveText, bool bCompleted);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> Check_Done;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Objective;

	/** 목표 문구 줄바꿈 기준 폭(px). 0이면 부모 슬롯이 준 폭에서 접는다. */
	UPROPERTY(EditAnywhere, Category = "Quest|Layout", meta = (ClampMin = "0.0"))
	float ObjectiveWrapTextAt = 0.f;

	virtual void NativeOnInitialized() override;
};
