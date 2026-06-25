// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Characters/NPC/BarCustomerNPC.h"
#include "Guest/Data/DataAssets/GBarNPCDialogueDataAsset.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ABarCustomerNPC::ABarCustomerNPC()
{
    PrimaryActorTick.bCanEverTick = false;

    // NPC 상반신을 정면에서 바라보는 기본 위치.
    // 에디터에서 NPC마다 좌석에 맞게 조정할 것.
    // NPC 앞쪽 150cm, 높이 130cm에서 NPC 얼굴을 바라보는 카메라.
    // 에디터에서 NPC마다 좌석 위치에 맞게 조정할 것.
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
    if (!BarDialogueAsset) return;

    UGameInstance* GI = GetGameInstance();
    if (!GI) return;

    UGuestUISubsystem* UISys = GI->GetSubsystem<UGuestUISubsystem>();
    if (!UISys) return;

    UISys->OpenBarDialogue(BarDialogueAsset->ToBarDialogueData(), this);
}

void ABarCustomerNPC::EndBarDialogue()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn) return;

    PC->SetViewTargetWithBlend(PlayerPawn, CameraBlendTime);
}
