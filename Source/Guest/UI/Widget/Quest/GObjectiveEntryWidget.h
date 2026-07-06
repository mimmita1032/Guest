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
};
