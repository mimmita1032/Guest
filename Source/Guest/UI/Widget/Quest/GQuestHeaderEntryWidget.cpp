// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GQuestHeaderEntryWidget.h"
#include "Guest/Utils/GuestBlueprintLibrary.h"
#include "Components/TextBlock.h"

void UGQuestHeaderEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UGuestBlueprintLibrary::ApplyAutoWrap(Text_QuestName, HeaderWrapTextAt);
	UGuestBlueprintLibrary::ApplyAutoWrap(Text_Description, HeaderWrapTextAt);
}

void UGQuestHeaderEntryWidget::Setup(const FString& InQuestName, const FString& InDescription)
{
	if (Text_QuestName)
	{
		Text_QuestName->SetText(FText::FromString(InQuestName));
	}

	if (Text_Description)
	{
		Text_Description->SetText(FText::FromString(InDescription));
	}
}
