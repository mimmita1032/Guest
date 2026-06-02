// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GSpacetimeSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Guest/Utils/GLog.h"

void UGSpacetimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SpacetimeDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Core/Data/DT_SpacetimeData")));    
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
bool UGSpacetimeSubsystem::SearchSpacetime(int32 Year, int32 AreaCode, FSpacetimeData& OutData)
{
	if (!SpacetimeDataTable) return false;

	TArray<FSpacetimeData*> AllRows;
	SpacetimeDataTable->GetAllRows<FSpacetimeData>(TEXT("SpacetimeSearch"), AllRows);

	for (const auto Row : AllRows)
	{
		if (Row->TargetYear == Year && Row->AreaCode == AreaCode)
		{
			OutData = *Row;
			return true;
		}
	}
	return false;
}

void UGSpacetimeSubsystem::ExecuteTravel(const FSpacetimeData& TargetData)
{
	if (TargetData.LevelName.IsNone()) return;

	G_LOG(TEXT("시공간 이동 시작: %s (%.1f초 후 전환)"), *TargetData.PlaceName.ToString(), TravelFadeDelay);

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
	UGameplayStatics::OpenLevel(GetWorld(), PendingTravelData.LevelName);
}

void UGSpacetimeSubsystem::ReturnToBase(FName BaseLevelName)
{
	if (BaseLevelName.IsNone()) return;

	G_LOG(TEXT("기지로 귀가 시작"));
	UGameplayStatics::OpenLevel(GetWorld(), BaseLevelName);
}
#pragma endregion
#pragma region Time
void UGSpacetimeSubsystem::UpdateWorldTime(float DeltaTime)
{
	if (!bUseRealTimeSync)
	{
		return;
	}

	FDateTime CurrentRealTime = FDateTime::Now();

	const int32 Hour   = CurrentRealTime.GetHour();
	const int32 Minute = CurrentRealTime.GetMinute();
	const int32 Second = CurrentRealTime.GetSecond();

	// 분이 바뀌지 않으면 브로드캐스트 생략
	if (Hour == LastBroadcastHour && Minute == LastBroadcastMinute)
	{
		return;
	}
	LastBroadcastHour   = Hour;
	LastBroadcastMinute = Minute;

	CurrentTime = Hour + (Minute / 60.0f) + (Second / 3600.0f);

	OnTimeChanged.Broadcast(CurrentTime);
}

void UGSpacetimeSubsystem::SetWorldTime(float NewHour)
{
	// 디버그 UI(슬라이더)로 이 함수가 호출되는 순간, 현실 시간 동기화를 강제로 끔
	bUseRealTimeSync = false;
    
	CurrentTime = FMath::Clamp(NewHour, 0.0f, 23.99f);
    
	OnTimeChanged.Broadcast(CurrentTime);
    
	G_LOG(TEXT("디버그 UI 조작: 세계 시간이 %.2f시로 설정 (현실 시간 동기화 중지)"), CurrentTime);
}

void UGSpacetimeSubsystem::ResumeRealTimeSync()
{
	bUseRealTimeSync = true;
	G_LOG(TEXT("현실 시간 동기화"));
}
#pragma endregion