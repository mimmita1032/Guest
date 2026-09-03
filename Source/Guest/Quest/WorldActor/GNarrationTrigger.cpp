// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GNarrationTrigger.h"
#include "Guest/Data/DataAssets/GNarrationDataAsset.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/Utils/GLog.h"
#include "Kismet/GameplayStatics.h"

AGNarrationTrigger::AGNarrationTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

void AGNarrationTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (NarrationAsset.IsNull())
	{
		G_WARN(TEXT("GNarrationTrigger: NarrationAsset이 비어 있습니다. [%s]"), *GetName());
		return;
	}

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!GI) return;

	if (RequiredStoryProgress > 0)
	{
		const UGQuestSubsystem* QuestSys = GI->GetSubsystem<UGQuestSubsystem>();
		if (!QuestSys || QuestSys->GetStoryProgress() < RequiredStoryProgress)
		{
			return;
		}
	}

	UGuestUISubsystem* UISys = GI->GetSubsystem<UGuestUISubsystem>();
	if (!UISys) return;

	// 레벨이 열리는 프레임에 바로 띄우면 다른 위젯이 아직 스택에 붙는 중이라 밀릴 수 있다.
	// 한 프레임 뒤로 미뤄 레이아웃이 자리를 잡은 뒤 올린다.
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, [this, UISys]()
	{
		if (UGNarrationDataAsset* Loaded = NarrationAsset.LoadSynchronous())
		{
			G_LOG(TEXT("GNarrationTrigger: 나레이션 재생 [%s]"), *Loaded->GetName());
			UISys->OpenNarration(Loaded);
		}
	}, 0.1f, false);
}
