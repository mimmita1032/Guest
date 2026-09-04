// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/Quest/GObjectiveEntryWidget.h"
#include "Guest/Utils/GuestBlueprintLibrary.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

void UGObjectiveEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 목표 문구는 DataTable에서 오는 가변 길이다 — 전체화면에서 잘리지 않게 접는다.
	UGuestBlueprintLibrary::ApplyAutoWrap(Text_Objective, ObjectiveWrapTextAt);
}

void UGObjectiveEntryWidget::Setup(const FString& InObjectiveText, bool bCompleted)
{
	if (Text_Objective) Text_Objective->SetText(FText::FromString(InObjectiveText));
	if (Check_Done)
	{
		Check_Done->SetIsChecked(bCompleted);
		Check_Done->SetIsEnabled(false);
	}
}
