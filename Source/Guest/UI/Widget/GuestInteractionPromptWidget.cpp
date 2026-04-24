// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GuestInteractionPromptWidget.h"
#include "Guest/Components/Interaction/GInteractionComponent.h"
#include "GameFramework/Pawn.h"

void UGuestInteractionPromptWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (UGInteractionComponent* InteractionComp = OwningPawn->FindComponentByClass<UGInteractionComponent>())
		{
			InteractionComp->OnInteractTextChanged.AddDynamic(this, &UGuestInteractionPromptWidget::OnInteractTextChanged);
		}
	}
	
	SetVisibility(ESlateVisibility::Collapsed);
}

void UGuestInteractionPromptWidget::OnInteractTextChanged(const FText& NewText)
{
	if (NewText.IsEmpty())
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UpdatePromptText(NewText);
	}
}