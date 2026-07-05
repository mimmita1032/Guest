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
	// 데이터 테이블 기반 검색
	bool SearchSpacetime(int32 Year, int32 AreaCode, FSpacetimeData& OutData);

	// 실제 레벨 이동 명령
	void ExecuteTravel(const FSpacetimeData& TargetData);

	// 귀가 명령
	void ReturnToBase(FName BaseLevelName);

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