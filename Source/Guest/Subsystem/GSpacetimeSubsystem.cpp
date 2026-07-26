// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GSpacetimeSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"
#include "UObject/UObjectGlobals.h"
#include "Guest/Core/GameInstance/GuestGameInstance.h"
#include "Guest/Save/GuestMapPackageUtils.h"
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

	// 이동뿐 아니라 게임 시작·문 이동으로 들어온 레벨에서도 현재 좌표가 있어야 사진에 장소가 박힌다
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this, &UGSpacetimeSubsystem::HandlePostLoadMap);
}

void UGSpacetimeSubsystem::Deinitialize()
{
	if (PostLoadMapHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
		PostLoadMapHandle.Reset();
	}

	Super::Deinitialize();
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

	// 도착지가 곧 현재 좌표가 된다. OpenLevel 전에 확정해 두면
	// 레벨 로드 후 역조회(HandlePostLoadMap)가 이 값을 그대로 인정한다 —
	// 같은 레벨을 가리키는 행이 여럿일 때 어느 좌표로 왔는지는 이동 기록만이 안다.
	CurrentLocation = PendingTravelData;
	bHasCurrentLocation = true;

	if (UGuestGameInstance* GI = Cast<UGuestGameInstance>(GetGameInstance()))
	{
		GI->CarryPlayerStateAcrossTravel();
	}
	UGameplayStatics::OpenLevel(GetWorld(), PendingTravelData.LevelName);
}

bool UGSpacetimeSubsystem::FindRowByLevelName(FName LevelName, FSpacetimeData& OutData) const
{
	if (!SpacetimeDataTable || LevelName.IsNone()) return false;

	TArray<FSpacetimeData*> AllRows;
	SpacetimeDataTable->GetAllRows<FSpacetimeData>(TEXT("SpacetimeLevelLookup"), AllRows);

	for (const auto Row : AllRows)
	{
		if (Row->LevelName == LevelName)
		{
			OutData = *Row;
			return true;
		}
	}
	return false;
}

void UGSpacetimeSubsystem::EnsureCurrentLocation()
{
	if (bHasCurrentLocation) return;

	HandlePostLoadMap(GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr);
}

void UGSpacetimeSubsystem::HandlePostLoadMap(UWorld* LoadedWorld)
{
	if (!LoadedWorld || !LoadedWorld->PersistentLevel) return;

	// PIE에서는 패키지명에 UEDPIE_<n>_ 접두가 붙으므로 떼고 비교한다
	const FString PackagePath = GuestMapPackage::StripPIEFromPackagePath(
		LoadedWorld->PersistentLevel->GetOutermost()->GetName());
	const FName LoadedLevelName(*FPackageName::GetShortName(PackagePath));

	// 이동으로 확정된 좌표가 방금 로드된 레벨과 같으면 그대로 둔다
	if (bHasCurrentLocation && CurrentLocation.LevelName == LoadedLevelName)
	{
		return;
	}

	// ExecuteTravel을 거치지 않고 들어온 레벨 (게임 시작, 문 이동, 세이브 로드)
	FSpacetimeData Found;
	if (FindRowByLevelName(LoadedLevelName, Found))
	{
		CurrentLocation = Found;
		bHasCurrentLocation = true;
		G_LOG(TEXT("현재 좌표 확정(레벨 역조회): %s"), *CurrentLocation.PlaceName.ToString());
		return;
	}

	// DT에 없는 레벨 — 사진에 장소 정보가 비어 들어간다.
	// 촬영이 필요한 레벨이라면 DT_SpacetimeData에 행을 추가할 것.
	CurrentLocation = FSpacetimeData();
	bHasCurrentLocation = false;
	G_WARN(TEXT("현재 좌표 미확정 — DT_SpacetimeData에 '%s' 행이 없습니다. 이 레벨의 사진에는 장소 정보가 비어 들어갑니다."),
		*LoadedLevelName.ToString());
}

void UGSpacetimeSubsystem::ExportLocationSaveData(int32& OutYear, int32& OutAreaCode) const
{
	// 좌표 자체가 아니라 좌표를 찾는 열쇠(연도+구역)만 저장한다.
	// DT의 다른 필드가 바뀌어도 로드 시 최신 값을 따라간다.
	OutYear     = bHasCurrentLocation ? CurrentLocation.TargetYear : -1;
	OutAreaCode = bHasCurrentLocation ? CurrentLocation.AreaCode   : -1;
}

void UGSpacetimeSubsystem::ImportLocationSaveData(int32 InYear, int32 InAreaCode)
{
	// 좌표가 저장된 적 없는 구버전 세이브 — 레벨 로드 후 역조회에 맡긴다
	if (InYear < 0 || InAreaCode < 0) return;

	FSpacetimeData Found;
	if (SearchSpacetime(InYear, InAreaCode, Found) != ESpacetimeSearchResult::NoMatch)
	{
		CurrentLocation = Found;
		bHasCurrentLocation = true;
		G_LOG(TEXT("세이브 로드: 현재 좌표를 %s(으)로 복원"), *CurrentLocation.PlaceName.ToString());
	}
	else
	{
		G_WARN(TEXT("세이브 로드: 저장된 좌표(%d/%d)가 DT에 없습니다 — 레벨 이름으로 다시 찾습니다."), InYear, InAreaCode);
	}
}
#pragma endregion
#pragma region Time
void UGSpacetimeSubsystem::UpdateWorldTime(float DeltaTime)
{
	CurrentTime += (DeltaTime * TimeScale) / 3600.0f;

	if (CurrentTime >= 24.0f)
	{
		// 히치 등으로 DeltaTime이 크면 하루를 여러 번 넘길 수 있으므로 넘긴 만큼 센다
		const int32 DaysPassed = FMath::FloorToInt(CurrentTime / 24.0f);
		CurrentTime = FMath::Fmod(CurrentTime, 24.0f);

		CurrentDay += DaysPassed;
		OnDayChanged.Broadcast(CurrentDay);
		G_LOG(TEXT("자정 경과: %d일차"), CurrentDay);
	}

	OnTimeChanged.Broadcast(CurrentTime);
}

void UGSpacetimeSubsystem::AdvanceDay(int32 NumDays, float NewHour)
{
	if (NumDays <= 0)
	{
		G_WARN(TEXT("AdvanceDay: 진행 일수는 1 이상이어야 합니다 (요청 %d)"), NumDays);
		return;
	}

	CurrentDay += NumDays;

	if (NewHour >= 0.0f)
	{
		CurrentTime = FMath::Clamp(NewHour, 0.0f, 23.99f);
		OnTimeChanged.Broadcast(CurrentTime);
	}

	OnDayChanged.Broadcast(CurrentDay);
	G_LOG(TEXT("날짜 진행: %d일 경과 → %d일차 %.2f시"), NumDays, CurrentDay, CurrentTime);
}

void UGSpacetimeSubsystem::SetWorldTime(float NewHour)
{
	CurrentTime = FMath::Clamp(NewHour, 0.0f, 23.99f);

	OnTimeChanged.Broadcast(CurrentTime);

	G_LOG(TEXT("디버그 UI 조작: 세계 시간이 %.2f시로 설정"), CurrentTime);
}

void UGSpacetimeSubsystem::ExportTimeSaveData(float& OutCurrentHour, int32& OutCurrentDay) const
{
	OutCurrentHour = CurrentTime;
	OutCurrentDay  = CurrentDay;
}

void UGSpacetimeSubsystem::ImportTimeSaveData(float InCurrentHour, int32 InCurrentDay)
{
	// 시간이 저장된 적 없는 세이브(-1 센티널)는 현재 시간 유지
	if (InCurrentHour >= 0.0f)
	{
		CurrentTime = FMath::Clamp(InCurrentHour, 0.0f, 23.99f);
		OnTimeChanged.Broadcast(CurrentTime);
		G_LOG(TEXT("세이브 로드: 세계 시간을 %.2f시로 복원"), CurrentTime);
	}

	// 날짜도 마찬가지 — 날짜 개념이 없던 구버전 세이브는 1일차를 유지한다
	if (InCurrentDay > 0)
	{
		CurrentDay = InCurrentDay;
		OnDayChanged.Broadcast(CurrentDay);
		G_LOG(TEXT("세이브 로드: 날짜를 %d일차로 복원"), CurrentDay);
	}
}
#pragma endregion