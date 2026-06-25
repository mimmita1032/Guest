// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Characters/NPC/BarCustomerNPC.h"
#include "Guest/Data/DataAssets/GDialogueDataAsset.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
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

	PC->SetViewTargetWithBlend(PlayerPawn, CameraBlendTime);
}
