// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GQuestTrackerWidget.generated.h"

class UVerticalBox;
class UGQuestSubsystem;
class UGObjectiveEntryWidget;

UCLASS()
class GUEST_API UGQuestTrackerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> Box_QuestList;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	TSubclassOf<UGObjectiveEntryWidget> ObjectiveEntryClass;

private:
	UFUNCTION()
	void RefreshQuestList();

	UGQuestSubsystem* GetQuestSubsystem() const;
};
