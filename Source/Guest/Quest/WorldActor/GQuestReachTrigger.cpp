// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GQuestReachTrigger.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/Utils/GLog.h"
#include "Kismet/GameplayStatics.h"

AGQuestReachTrigger::AGQuestReachTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

void AGQuestReachTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TargetID.IsNone())
	{
		G_WARN(TEXT("GQuestReachTrigger: TargetID가 설정되지 않았습니다. [%s]"), *GetName());
		return;
	}

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UGQuestSubsystem* QuestSys = GI->GetSubsystem<UGQuestSubsystem>())
		{
			G_LOG(TEXT("GQuestReachTrigger: [%s] 도착 목표 갱신"), *TargetID.ToString());
			QuestSys->OnObjectiveUpdated.Broadcast(TargetID, 1);
		}
	}
}
