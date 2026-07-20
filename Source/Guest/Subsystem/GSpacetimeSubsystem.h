// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GSpacetimeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTravelStarted, const FSpacetimeData&, TargetData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChanged, float, CurrentHour);

UCLASS()
class GUEST_API UGSpacetimeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

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

	FSpacetimeData PendingTravelData;
	FTimerHandle TravelTimerHandle;
	bool bTravelInProgress = false;
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

	// 시간이 변할 때마다 다른 객체들에게 알리는 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Spacetime|Time")
	FOnTimeChanged OnTimeChanged;

	// 세이브용: 현재 세계 시간 내보내기
	void ExportTimeSaveData(float& OutCurrentHour) const;

	// 세이브용: 세계 시간 복원 (음수 = 저장된 적 없는 세이브 → 무시)
	void ImportTimeSaveData(float InCurrentHour);

protected:
	// 현재 시간 (0.0 ~ 24.0)
	UPROPERTY(VisibleInstanceOnly, Category = "Spacetime|Time")
	float CurrentTime = 9.0f;

	// 시간 흐름 배율 (게임 1시간 = 현실 3600/TimeScale초. 예: 60이면 하루가 현실 24분)
	UPROPERTY(EditAnywhere, Category = "Spacetime|Time")
	float TimeScale = 60.0f;
#pragma endregion
	
	

protected:
	// 마스터 데이터 테이블
	UPROPERTY()
	TObjectPtr<UDataTable> SpacetimeDataTable;
};