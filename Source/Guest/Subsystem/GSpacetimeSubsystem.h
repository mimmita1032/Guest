// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GSpacetimeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTravelStarted, const FSpacetimeData&, TargetData);

UCLASS()
class GUEST_API UGSpacetimeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	// 데이터 테이블 기반 검색
	bool SearchSpacetime(int32 Year, int32 AreaCode, FSpacetimeData& OutData);

	// 실제 레벨 이동 명령
	void ExecuteTravel(const FSpacetimeData& TargetData);

	// 귀가 명령
	void ReturnToBase(FName BaseLevelName);

public:
	// 이동 시작 시 발생하는 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Spacetime")
	FOnTravelStarted OnTravelStarted;

protected:
	// 마스터 데이터 테이블
	UPROPERTY()
	TObjectPtr<UDataTable> SpacetimeDataTable;
};