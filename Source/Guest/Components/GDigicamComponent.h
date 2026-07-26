// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GDigicamComponent.generated.h"

// 연도/구역/매칭 결과가 바뀔 때마다 브로드캐스트 — UI 탭에서 구독
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnDigicamSearchUpdated,
	int32, Year, int32, AreaCode, FSpacetimeData, MatchedData, EDigicamState, State,
	ESpacetimeSearchResult, SearchResult);

// 이동 거부 시 UI 피드백용 (사유 = 마지막 검색 결과).
// 이름은 셔터가 이동을 겸하던 시절의 것 — 기존 BP 바인딩을 깨지 않으려 그대로 둔다.
// 실제로는 HandleTravel이 거부됐을 때만 발생한다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShutterDenied, ESpacetimeSearchResult, Reason);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUEST_API UGDigicamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGDigicamComponent();

	// 수거 작업 시작 (디카 꺼내기)
	void ActivateDigicam();

	// 수거 작업 중단 (디카 넣기)
	void DeactivateDigicam();

	// 입력 처리
	void HandleVerticalInput(float Value);   // 상/하: 숫자 조절
	void HandleHorizontalInput(float Value); // 좌/우: 항목 이동
	// 셔터: 지금 있는 곳을 촬영한다 (이동과 무관 — 언제든 누를 수 있다)
	UFUNCTION(BlueprintCallable, Category = "Digicam")
	void HandleShutter();

	// 이동 실행: 좌표가 맞춰진 상태(ReadyToSnap)에서만 동작한다.
	// 셔터와 분리돼 있다 — 사진을 찍는 것과 시공간을 건너는 것은 다른 행위다
	UFUNCTION(BlueprintCallable, Category = "Digicam")
	void HandleTravel();

	// UI 탭에서 현재 상태 조회용
	UFUNCTION(BlueprintPure, Category = "Digicam")
	EDigicamState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Digicam")
	int32 GetSelectedYear() const { return SelectedYear; }

	UFUNCTION(BlueprintPure, Category = "Digicam")
	int32 GetSelectedAreaCode() const { return SelectedAreaCode; }

	UFUNCTION(BlueprintPure, Category = "Digicam")
	FSpacetimeData GetCurrentMatchedData() const { return CurrentMatchedData; }

	UFUNCTION(BlueprintPure, Category = "Digicam")
	ESpacetimeSearchResult GetLastSearchResult() const { return LastSearchResult; }

	// 스페이스타임 탭에서 구독
	UPROPERTY(BlueprintAssignable, Category = "Digicam")
	FOnDigicamSearchUpdated OnDigicamSearchUpdated;

	// 셔터를 눌렀지만 이동이 거부됐을 때 (미매칭/잠김/비활성) — UI 연출용
	UPROPERTY(BlueprintAssignable, Category = "Digicam")
	FOnShutterDenied OnShutterDenied;

protected:
	virtual void BeginPlay() override;

	void UpdateSearch();
	void BroadcastSearchState();

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Digicam")
	EDigicamState CurrentState;

	UPROPERTY(EditAnywhere, Category = "Digicam")
	int32 SelectedYear;

	UPROPERTY(EditAnywhere, Category = "Digicam")
	int32 SelectedAreaCode;

	FSpacetimeData CurrentMatchedData;

	ESpacetimeSearchResult LastSearchResult = ESpacetimeSearchResult::NoMatch;

	UPROPERTY(EditAnywhere, Category = "Digicam|Settings")
	float InputDelay = 0.15f;

	float LastVerticalInputTime = 0.0f;
	float LastHorizontalInputTime = 0.0f;
};