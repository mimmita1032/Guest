// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GBarDialogueWidget.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/Characters/NPC/BarCustomerNPC.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"
#include "GameFramework/PlayerController.h"

void UGBarDialogueWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	UGuestUISubsystem* UISys = GetUISubsystem();
	if (!UISys) return;

	if (ABarCustomerNPC* NPC = Cast<ABarCustomerNPC>(UISys->GetPendingDialogueNPCActor()))
	{
		BlendToNPCCamera(NPC);
	}

	StartDialogueSession(UISys->GetPendingDialogueAsset());
}

void UGBarDialogueWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	UGuestUISubsystem* UISys = GetUISubsystem();
	if (UISys)
	{
		if (ABarCustomerNPC* NPC = Cast<ABarCustomerNPC>(UISys->GetPendingDialogueNPCActor()))
		{
			NPC->EndBarDialogue();
		}
		UISys->NotifyWidgetDeactivated(GuestGameplayTags::TAG_WidgetStack_BarDialogue);
	}

	ResetDialogueSession();
}

void UGBarDialogueWidget::BlendToNPCCamera(ABarCustomerNPC* NPC)
{
	if (!NPC) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	PC->SetViewTargetWithBlend(NPC, 0.5f);
}
