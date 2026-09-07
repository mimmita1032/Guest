// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GSpacetimeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTravelStarted, const FSpacetimeData&, TargetData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChanged, float, CurrentHour);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayChanged, int32, CurrentDay);

UCLASS()
class GUEST_API UGSpacetimeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

#pragma region Space
public:
	// 데이터 테이블 기반 검색 + 스토리 잠금 판정 (단일 진실 공급원)
	// Locked일 때도 OutData는 채워짐 — 티저 노출 여부는 UI가 결정
	UFUNCTION(BlueprintCallable, Category = "Spacetime")
	ESpacetimeSearchResult SearchSpacetime(int32 Year, int32 AreaCode, FSpacetimeData& OutData) const;

	// 스토리 진행도 대비 해금 여부 (GQuestSubsystem::GetStoryProgress()와 대조)
	UFUNCTION(BlueprintPure, Category = "Spacetime")
	bool IsSpacetimeUnlocked(const FSpacetimeData& Data) const;

	// 실제 레벨 이동 명령 — 잠금 재검증 + 이동 중 재진입 차단
	UFUNCTION(BlueprintCallable, Category = "Spacetime")
	void ExecuteTravel(const FSpacetimeData& TargetData);

	/**
	 * 해금 여부를 보지 않고 이동한다.
	 *
	 * 사망 복귀처럼 "게이팅과 무관하게 반드시 보내야 하는" 경로에 쓴다.
	 * 주점은 진행도 1을 요구하는데, 그 전에 죽으면 돌아갈 곳이 없어진다.
	 */
	void ExecuteTravelIgnoringLock(const FSpacetimeData& TargetData);

	// 지금 플레이어가 있는 시공간 좌표.
	// 촬영한 사진의 장소·연도·SubjectID가 전부 여기서 나온다 — 사진의 "언제 어디서"의 단일 출처.
	UFUNCTION(BlueprintPure, Category = "Spacetime")
	FSpacetimeData GetCurrentLocation() const { return CurrentLocation; }

	// 현재 좌표가 확정돼 있는가. 미확정이면 사진에 장소 정보가 비어 들어간다
	UFUNCTION(BlueprintPure, Category = "Spacetime")
	bool HasCurrentLocation() const { return bHasCurrentLocation; }

	// 좌표가 아직 미확정이면 지금 월드를 기준으로 확정을 시도한다.
	// PostLoadMapWithWorld는 PIE 시작처럼 맵이 GameInstance보다 먼저 준비되는 경로에서
	// 첫 레벨에 대해 오지 않을 수 있다 — 촬영처럼 좌표가 실제로 필요한 시점의 안전망.
	UFUNCTION(BlueprintCallable, Category = "Spacetime")
	void EnsureCurrentLocation();

	// 세이브용: 현재 좌표를 연도/구역으로 내보낸다 (미확정이면 둘 다 -1)
	void ExportLocationSaveData(int32& OutYear, int32& OutAreaCode) const;

	// 세이브용: 연도/구역으로 현재 좌표를 다시 찾아 복원 (-1이면 무시)
	void ImportLocationSaveData(int32 InYear, int32 InAreaCode);

	// 이동 진행 중 여부 (페이드 중 셔터 연타 등 중복 이동 방지, UI 조회용)
	UFUNCTION(BlueprintPure, Category = "Spacetime")
	bool IsTravelInProgress() const { return bTravelInProgress; }

	// 이동 시작 시 발생하는 이벤트 (페이드 연출용 — OpenLevel보다 TravelFadeDelay초 먼저 발생)
	UPROPERTY(BlueprintAssignable, Category = "Spacetime")
	FOnTravelStarted OnTravelStarted;

	// 페이드 아웃 후 실제 OpenLevel까지의 지연 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacetime")
	float TravelFadeDelay = 1.0f;

private:
	void DoTravel();

	// 맵 로드 완료 시 현재 좌표를 확정한다.
	// 이동으로 온 경우는 이미 확정돼 있고, 게임 시작·문 이동·세이브 로드처럼
	// ExecuteTravel을 거치지 않은 경로는 레벨 이름으로 역조회해 채운다.
	void HandlePostLoadMap(UWorld* LoadedWorld);

	// 레벨 이름으로 좌표 행을 찾는다 (같은 레벨을 가리키는 행이 여럿이면 첫 번째)
	bool FindRowByLevelName(FName LevelName, FSpacetimeData& OutData) const;

	FSpacetimeData PendingTravelData;
	FTimerHandle TravelTimerHandle;
	bool bTravelInProgress = false;

	FSpacetimeData CurrentLocation;
	bool bHasCurrentLocation = false;

	FDelegateHandle PostLoadMapHandle;
#pragma endregion

#pragma region Time
public:
	// 매 프레임 시간을 업데이트하는 함수
	void UpdateWorldTime(float DeltaTime);

	// 시간을 특정 값으로 강제 설정하는 함수
	void SetWorldTime(float NewHour);

	// 현재 시간을 반환하는 함수
	UFUNCTION(BlueprintCallable, Category = "Spacetime|Time")
	float GetCurrentHour() const { return CurrentTime; }

	// 현재 날짜 (1부터 시작 — 게임 시작일이 1일차)
	UFUNCTION(BlueprintCallable, Category = "Spacetime|Time")
	int32 GetCurrentDay() const { return CurrentDay; }

	// 날짜를 강제로 진행시킨다 (나레이션 후 "하루 뒤" 같은 스크립트 전환용).
	// NewHour가 0 이상이면 시각도 함께 설정한다 — 예: AdvanceDay(1, 9.f) = 다음 날 아침 9시
	UFUNCTION(BlueprintCallable, Category = "Spacetime|Time")
	void AdvanceDay(int32 NumDays = 1, float NewHour = -1.0f);

	// 시간이 변할 때마다 다른 객체들에게 알리는 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Spacetime|Time")
	FOnTimeChanged OnTimeChanged;

	// 날짜가 바뀔 때 발생 (자정 경과 또는 AdvanceDay)
	UPROPERTY(BlueprintAssignable, Category = "Spacetime|Time")
	FOnDayChanged OnDayChanged;

	// 세이브용: 현재 세계 시간/날짜 내보내기
	void ExportTimeSaveData(float& OutCurrentHour, int32& OutCurrentDay) const;

	// 세이브용: 세계 시간/날짜 복원 (음수 = 저장된 적 없는 세이브 → 각각 무시)
	void ImportTimeSaveData(float InCurrentHour, int32 InCurrentDay);

protected:
	// 현재 시간 (0.0 ~ 24.0)
	UPROPERTY(VisibleInstanceOnly, Category = "Spacetime|Time")
	float CurrentTime = 9.0f;

	// 현재 날짜 (1일차부터 시작)
	UPROPERTY(VisibleInstanceOnly, Category = "Spacetime|Time")
	int32 CurrentDay = 1;

	// 시간 흐름 배율 (게임 1시간 = 현실 3600/TimeScale초. 예: 60이면 하루가 현실 24분)
	UPROPERTY(EditAnywhere, Category = "Spacetime|Time")
	float TimeScale = 60.0f;
#pragma endregion
	
	

protected:
	// 마스터 데이터 테이블
	UPROPERTY()
	TObjectPtr<UDataTable> SpacetimeDataTable;
};