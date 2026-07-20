// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GSpacetimeSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Guest/Core/GameInstance/GuestGameInstance.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/Utils/GLog.h"

void UGSpacetimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SpacetimeDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_SpacetimeData")));
	if (SpacetimeDataTable)
	{
		G_LOG(TEXT("시공간 데이터 테이블 로드 성공"));
	}
	else
	{
		G_ERR(TEXT("시공간 데이터 테이블 로드 실패! 경로를 다시 체크."));
	}
}
#pragma region Space
ESpacetimeSearchResult UGSpacetimeSubsystem::SearchSpacetime(int32 Year, int32 AreaCode, FSpacetimeData& OutData) const
{
	if (!SpacetimeDataTable) return ESpacetimeSearchResult::NoMatch;

	TArray<FSpacetimeData*> AllRows;
	SpacetimeDataTable->GetAllRows<FSpacetimeData>(TEXT("SpacetimeSearch"), AllRows);

	for (const auto Row : AllRows)
	{
		if (Row->TargetYear == Year && Row->AreaCode == AreaCode)
		{
			OutData = *Row;
			return IsSpacetimeUnlocked(OutData) ? ESpacetimeSearchResult::Found : ESpacetimeSearchResult::Locked;
		}
	}
	return ESpacetimeSearchResult::NoMatch;
}

bool UGSpacetimeSubsystem::IsSpacetimeUnlocked(const FSpacetimeData& Data) const
{
	if (const UGQuestSubsystem* QuestSys = GetGameInstance()->GetSubsystem<UGQuestSubsystem>())
	{
		return QuestSys->GetStoryProgress() >= Data.RequiredStoryProgress;
	}

	// 퀘스트 서브시스템을 못 얻는 비정상 상황 — 잠금 우선(안전 기본값)
	return Data.RequiredStoryProgress <= 0;
}

void UGSpacetimeSubsystem::ExecuteTravel(const FSpacetimeData& TargetData)
{
	if (TargetData.LevelName.IsNone()) return;
	if (bTravelInProgress) return;

	if (!IsSpacetimeUnlocked(TargetData))
	{
		G_WARN(TEXT("시공간 이동 거부 — 아직 해금되지 않은 좌표: %s"), *TargetData.PlaceName.ToString());
		return;
	}

	G_LOG(TEXT("시공간 이동 시작: %s (%.1f초 후 전환)"), *TargetData.PlaceName.ToString(), TravelFadeDelay);

	bTravelInProgress = true;
	PendingTravelData = TargetData;

	// 페이드 연출 시작 신호 — Blueprint에서 이 시점에 페이드 아웃 재생
	OnTravelStarted.Broadcast(TargetData);

	GetWorld()->GetTimerManager().SetTimer(
		TravelTimerHandle,
		this,
		&UGSpacetimeSubsystem::DoTravel,
		TravelFadeDelay,
		false
	);
}

void UGSpacetimeSubsystem::DoTravel()
{
	// bTravelInProgress는 페이드 딜레이 동안의 재진입만 막으면 되므로 여기서 해제.
	// 이 서브시스템은 GameInstance 스코프라 OpenLevel로 월드가 바뀌어도 살아남아 리셋 없이는 이후 이동이 영구히 막힘.
	bTravelInProgress = false;

	if (UGuestGameInstance* GI = Cast<UGuestGameInstance>(GetGameInstance()))
	{
		GI->CarryPlayerStateAcrossTravel();
	}
	UGameplayStatics::OpenLevel(GetWorld(), PendingTravelData.LevelName);
}
#pragma endregion
#pragma region Time
void UGSpacetimeSubsystem::UpdateWorldTime(float DeltaTime)
{
	CurrentTime += (DeltaTime * TimeScale) / 3600.0f;

	if (CurrentTime >= 24.0f)
	{
		CurrentTime = FMath::Fmod(CurrentTime, 24.0f);
	}

	OnTimeChanged.Broadcast(CurrentTime);
}

void UGSpacetimeSubsystem::SetWorldTime(float NewHour)
{
	CurrentTime = FMath::Clamp(NewHour, 0.0f, 23.99f);

	OnTimeChanged.Broadcast(CurrentTime);

	G_LOG(TEXT("디버그 UI 조작: 세계 시간이 %.2f시로 설정"), CurrentTime);
}

void UGSpacetimeSubsystem::ExportTimeSaveData(float& OutCurrentHour) const
{
	OutCurrentHour = CurrentTime;
}

void UGSpacetimeSubsystem::ImportTimeSaveData(float InCurrentHour)
{
	// 시간이 저장된 적 없는 세이브(-1 센티널)는 현재 시간 유지
	if (InCurrentHour < 0.0f)
	{
		return;
	}

	CurrentTime = FMath::Clamp(InCurrentHour, 0.0f, 23.99f);
	OnTimeChanged.Broadcast(CurrentTime);

	G_LOG(TEXT("세이브 로드: 세계 시간을 %.2f시로 복원"), CurrentTime);
}
#pragma endregion