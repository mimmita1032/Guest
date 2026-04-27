// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GDigicamComponent.generated.h"

// 연도/구역/매칭 결과가 바뀔 때마다 브로드캐스트 — UI 탭에서 구독
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDigicamSearchUpdated,
	int32, Year, int32, AreaCode, FSpacetimeData, MatchedData, EDigicamState, State);

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
	UFUNCTION(BlueprintCallable, Category = "Digicam")
	void HandleShutter();                    // 셔터: 수거 실행

	// UI 탭에서 현재 상태 조회용
	UFUNCTION(BlueprintPure, Category = "Digicam")
	EDigicamState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Digicam")
	int32 GetSelectedYear() const { return SelectedYear; }

	UFUNCTION(BlueprintPure, Category = "Digicam")
	int32 GetSelectedAreaCode() const { return SelectedAreaCode; }

	UFUNCTION(BlueprintPure, Category = "Digicam")
	FSpacetimeData GetCurrentMatchedData() const { return CurrentMatchedData; }

	// 스페이스타임 탭에서 구독
	UPROPERTY(BlueprintAssignable, Category = "Digicam")
	FOnDigicamSearchUpdated OnDigicamSearchUpdated;

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

	// '저기'의 좌표들이 담긴 데이터 테이블
	UPROPERTY(EditAnywhere, Category = "Digicam")
	TObjectPtr<UDataTable> SpacetimeTable;

	// 소낙의 집(주점) 레벨 이름. 에디터에서 직접 입력.
	UPROPERTY(EditAnywhere, Category = "Digicam|Settings")
	FName BaseLevelName;

	bool IsAtBaseLevel() const;

	FSpacetimeData CurrentMatchedData;

	UPROPERTY(EditAnywhere, Category = "Digicam|Settings")
	float InputDelay = 0.15f;

	float LastVerticalInputTime = 0.0f;
	float LastHorizontalInputTime = 0.0f;
};