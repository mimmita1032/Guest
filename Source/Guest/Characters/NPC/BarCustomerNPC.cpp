// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Characters/NPC/BarCustomerNPC.h"
#include "Guest/Data/DataAssets/GDialogueDataAsset.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ABarCustomerNPC::ABarCustomerNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	DialogueCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DialogueCamera"));
	DialogueCamera->SetupAttachment(GetRootComponent());
	DialogueCamera->SetRelativeLocation(FVector(150.f, 0.f, 130.f));
	DialogueCamera->SetRelativeRotation(FRotator(-10.f, 180.f, 0.f));

	DialogueTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("DialogueTrigger"));
	DialogueTrigger->SetupAttachment(GetRootComponent());
	DialogueTrigger->SetBoxExtent(FVector(120.f, 120.f, 90.f));
	DialogueTrigger->SetRelativeLocation(FVector(60.f, 0.f, 0.f));
	DialogueTrigger->SetCollisionProfileName(TEXT("Trigger"));
	DialogueTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABarCustomerNPC::OnTriggerBeginOverlap);
}

void ABarCustomerNPC::BeginPlay()
{
	Super::BeginPlay();

	if (UGQuestSubsystem* QuestSys = GetGameInstance()->GetSubsystem<UGQuestSubsystem>())
	{
		QuestSys->OnQuestListChanged.AddDynamic(this, &ABarCustomerNPC::HandleQuestListChanged);
	}
	ApplyStoryProgressVisibility();
}

void ABarCustomerNPC::HandleQuestListChanged()
{
	ApplyStoryProgressVisibility();
}

void ABarCustomerNPC::ApplyStoryProgressVisibility()
{
	// 0이면 게이팅 없음 — 항상 등장 상태 유지
	if (RequiredStoryProgress <= 0) return;

	const UGQuestSubsystem* QuestSys = GetGameInstance()->GetSubsystem<UGQuestSubsystem>();
	const bool bUnlocked = QuestSys && QuestSys->GetStoryProgress() >= RequiredStoryProgress;

	SetActorHiddenInGame(!bUnlocked);
	SetActorEnableCollision(bUnlocked);
}

void ABarCustomerNPC::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsDialogueActive) return;
	if (!OtherActor || !OtherActor->IsA<ACharacter>()) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || PC->GetPawn() != OtherActor) return;

	StartBarDialogue();
}

void ABarCustomerNPC::Interact_Implementation(AActor* Interactor)
{
	if (!DialogueAsset) return;
	if (bIsDialogueActive) return;

	UGuestUISubsystem* UISys = GetGameInstance()->GetSubsystem<UGuestUISubsystem>();
	if (!UISys) return;

	UISys->OpenNPCDialogue(DialogueAsset);
}

FText ABarCustomerNPC::GetInteractText_Implementation() const
{
	return FText::FromString(TEXT("대화하기"));
}

void ABarCustomerNPC::StartBarDialogue()
{
	if (bIsDialogueActive || !DialogueAsset) return;
	bIsDialogueActive = true;

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

	// Talk 목표는 대화 노드(FDialogueNode::TalkObjectiveID)가 발화한다.
	// 여기서 쏘면 바 대화 경로에서만 달성되고, E로 연 NPC 대화에서는 달성되지 않는다.

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
	bIsDialogueActive = false;
	if (APlayerController* PC = CachedPC.Get())
	{
		PC->ResetIgnoreMoveInput();
	}
}
