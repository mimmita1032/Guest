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
		G_ERR(TEXT("시공간 데이터 테이블 로드 실패! 경로를 확인하세요."));
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

	G_LOG(TEXT("시공간 이동 실행: %s"), *TargetData.PlaceName.ToString());
	
	// 이동 전 이벤트 전파 (화면 페이드, 사운드 등 연출용)
	OnTravelStarted.Broadcast(TargetData);

	UGameplayStatics::OpenLevel(GetWorld(), TargetData.LevelName);
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
	// 1. 증가할 시간 계산 (초 단위 델타타임을 시간 단위로 변환)
	float AddedHour = (DeltaTime * TimeScale) / 3600.0f;
	
	// 2. 현재 시간에 누적
	CurrentTime += AddedHour;

	// 3. 24시가 넘어가면 0시로 초기화 (24시간 순환)
	if (CurrentTime >= 24.0f)
	{
		CurrentTime -= 24.0f;
	}

	// 4. 시간 변경되었음!
	OnTimeChanged.Broadcast(CurrentTime);
}

void UGSpacetimeSubsystem::SetWorldTime(float NewHour)
{
	// 1. 입력된 시간을 0.0 ~ 23.99 사이로 강제 고정
	CurrentTime = FMath::Clamp(NewHour, 0.0f, 23.99f);
	
	// 2. 시간 변경되었음!
	OnTimeChanged.Broadcast(CurrentTime);
	
	// 3. 디버깅 UI추가 후 사용할겁니다
	G_LOG(TEXT("세계 시간이 %.2f시로 설정되었습니다."), CurrentTime);
}
#pragma endregion