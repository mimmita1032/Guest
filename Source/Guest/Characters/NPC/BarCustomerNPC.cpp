// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Characters/NPC/BarCustomerNPC.h"
#include "Guest/Data/DataAssets/GDialogueDataAsset.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ABarCustomerNPC::ABarCustomerNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	DialogueCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DialogueCamera"));
	DialogueCamera->SetupAttachment(GetRootComponent());
	DialogueCamera->SetRelativeLocation(FVector(150.f, 0.f, 130.f));
	DialogueCamera->SetRelativeRotation(FRotator(-10.f, 180.f, 0.f));
}

void ABarCustomerNPC::Interact_Implementation(AActor* Interactor)
{
	StartBarDialogue();
}

FText ABarCustomerNPC::GetInteractText_Implementation() const
{
	return FText::FromString(TEXT("대화하기"));
}

void ABarCustomerNPC::StartBarDialogue()
{
	if (!DialogueAsset) return;

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UGuestUISubsystem* UISys = GI->GetSubsystem<UGuestUISubsystem>();
	if (!UISys) return;

	UISys->OpenBarDialogue(DialogueAsset, this);
}

void ABarCustomerNPC::EndBarDialogue()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	if (!TalkObjectiveID.IsNone())
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UGQuestSubsystem* QuestSys = GI->GetSubsystem<UGQuestSubsystem>())
			{
				QuestSys->OnObjectiveUpdated.Broadcast(TalkObjectiveID, 1);
			}
		}
	}

	// 블렌드 중 이동 잠금 후 타이머로 복구
	PC->SetIgnoreMoveInput(true);
	CachedPC = PC;

	PC->SetViewTargetWithBlend(PlayerPawn, CameraBlendTime);

	GetWorldTimerManager().SetTimer(
		CameraRestoreTimerHandle,
		this, &ABarCustomerNPC::RestorePlayerInput,
		CameraBlendTime, false);
}

void ABarCustomerNPC::RestorePlayerInput()
{
	if (APlayerController* PC = CachedPC.Get())
	{
		PC->ResetIgnoreMoveInput();
	}
}
