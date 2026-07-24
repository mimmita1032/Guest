// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestNPCBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Guest/AI/Controllers/GuestAIController.h"
#include "Guest/Data/DataAssets/GNPCScheduleDataAsset.h"
#include "Guest/Data/DataAssets/GDialogueDataAsset.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/Subsystem/GSpacetimeSubsystem.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/Utils/GLog.h"

AGuestNPCBase::AGuestNPCBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AGuestAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 기본값은 배회하는 일반 주민
	NPCType = ENPCType::Ambient;
}

void AGuestNPCBase::BeginPlay()
{
	Super::BeginPlay();

	if (UGQuestSubsystem* QuestSys = GetGameInstance()->GetSubsystem<UGQuestSubsystem>())
	{
		QuestSys->OnQuestListChanged.AddDynamic(this, &AGuestNPCBase::HandleQuestListChanged);
	}
	ApplyStoryProgressVisibility();

	if (!ScheduleDataAsset) return;

	// GSpacetimeSubsystem의 OnTimeChanged 이벤트에 바인딩
	if (UGSpacetimeSubsystem* Spacetime = GetGameInstance()->GetSubsystem<UGSpacetimeSubsystem>())
	{
		Spacetime->OnTimeChanged.AddDynamic(this, &AGuestNPCBase::HandleTimeChanged);

		// 게임 시작 시점의 현재 시각으로 즉시 스케줄 적용
		ApplySchedule(Spacetime->GetCurrentHour());

		G_LOG(TEXT("[NPC Schedule] %s — 스케줄 등록 완료 (항목 수: %d)"),
			*GetName(), ScheduleDataAsset->Schedule.Num());
	}
}


void AGuestNPCBase::Interact_Implementation(AActor* Interactor)
{
	if (!DialogueAsset) return;

	UGuestUISubsystem* UISys = GetGameInstance()->GetSubsystem<UGuestUISubsystem>();
	if (!UISys) return;

	UISys->OpenNPCDialogue(DialogueAsset);
}

void AGuestNPCBase::HandleTimeChanged(float CurrentHour)
{
	ApplySchedule(CurrentHour);
}

void AGuestNPCBase::HandleQuestListChanged()
{
	ApplyStoryProgressVisibility();
}

void AGuestNPCBase::ApplyStoryProgressVisibility()
{
	// 0이면 게이팅 없음 — 항상 등장 상태 유지
	if (RequiredStoryProgress <= 0) return;

	const UGQuestSubsystem* QuestSys = GetGameInstance()->GetSubsystem<UGQuestSubsystem>();
	const bool bUnlocked = QuestSys && QuestSys->GetStoryProgress() >= RequiredStoryProgress;

	SetActorHiddenInGame(!bUnlocked);
	SetActorEnableCollision(bUnlocked);
}

void AGuestNPCBase::ApplySchedule(float CurrentHour)
{
	if (!ScheduleDataAsset) return;

	AGuestAIController* AIC = Cast<AGuestAIController>(GetController());
	if (!AIC) return;

	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB) return;

	const FNPCScheduleEntry* ActiveEntry = ScheduleDataAsset->FindActiveEntry(CurrentHour);

	if (ActiveEntry)
	{
		// 스케줄 항목이 바뀐 경우에만 목적지를 갱신하고 도착 플래그를 리셋
		if (ActiveEntry->DestinationTag != LastActiveScheduleTag)
		{
			// 레벨에서 태그가 일치하는 웨이포인트 액터를 탐색
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsWithTag(this, ActiveEntry->DestinationTag, FoundActors);

			if (FoundActors.IsEmpty())
			{
				G_WARN(TEXT("[NPC Schedule] %s — 목적지 태그 '%s'에 해당하는 액터를 찾지 못했습니다."),
					*GetName(), *ActiveEntry->DestinationTag.ToString());
				return;
			}

			const FVector Destination = FoundActors[0]->GetActorLocation();
			BB->SetValueAsVector(AGuestAIController::BB_ScheduledDestination, Destination);
			BB->SetValueAsBool(AGuestAIController::BB_bHasSchedule, true);
			BB->SetValueAsBool(AGuestAIController::BB_bScheduleArrived, false);

			LastActiveScheduleTag = ActiveEntry->DestinationTag;

			G_LOG(TEXT("[NPC Schedule] %s — %02d:%02d 새 스케줄: '%s' → %s"),
				*GetName(), ActiveEntry->StartHour, ActiveEntry->StartMinute,
				*ActiveEntry->DestinationTag.ToString(),
				*Destination.ToString());
		}
	}
	else
	{
		// 스케줄 없는 시간대 — BB 클리어 후 기본 배회로 복귀
		BB->ClearValue(AGuestAIController::BB_ScheduledDestination);
		BB->SetValueAsBool(AGuestAIController::BB_bHasSchedule, false);
		BB->SetValueAsBool(AGuestAIController::BB_bScheduleArrived, false);

		LastActiveScheduleTag = NAME_None;
	}
}