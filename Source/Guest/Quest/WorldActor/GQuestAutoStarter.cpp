// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GQuestAutoStarter.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/Utils/GLog.h"
#include "Kismet/GameplayStatics.h"

AGQuestAutoStarter::AGQuestAutoStarter()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

void AGQuestAutoStarter::BeginPlay()
{
	Super::BeginPlay();

	if (QuestID.IsNone())
	{
		G_WARN(TEXT("GQuestAutoStarter: QuestID가 설정되지 않았습니다. [%s]"), *GetName());
		return;
	}

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UGQuestSubsystem* QuestSys = GI->GetSubsystem<UGQuestSubsystem>())
		{
			G_LOG(TEXT("GQuestAutoStarter: [%s] 자동 수락 시도"), *QuestID.ToString());
			QuestSys->AcceptQuest(QuestID);
		}
	}
}
