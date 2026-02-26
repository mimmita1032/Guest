// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "DGDigicamComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUEST_API UDGDigicamComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDGDigicamComponent();

	// 수거 작업 시작 (디카 꺼내기)
	void ActivateDigicam();

	// 수거 작업 중단 (디카 넣기)
	void DeactivateDigicam();

	// 입력 처리
	void HandleVerticalInput(float Value);   // 상/하: 숫자 조절
	void HandleHorizontalInput(float Value); // 좌/우: 항목 이동
	void HandleShutter();                    // 셔터: 수거 실행

protected:
	virtual void BeginPlay() override;

	// 현재 입력값에 맞는 '저기'의 데이터가 있는지 확인
	void UpdateSearch();

protected:
	// 디카 상태
	UPROPERTY(VisibleInstanceOnly, Category = "Digicam")
	EDigicamState CurrentState;

	// 현재 설정 중인 연도
	UPROPERTY(EditAnywhere, Category = "Digicam")
	int32 SelectedYear;

	// 현재 설정 중인 구역 코드
	UPROPERTY(EditAnywhere, Category = "Digicam")
	int32 SelectedAreaCode;

	// '저기'의 좌표들이 담긴 데이터 테이블
	UPROPERTY(EditAnywhere, Category = "Digicam")
	TObjectPtr<UDataTable> SpacetimeTable;

	// 위젯
	UPROPERTY(EditAnywhere, Category = "Digicam|UI")
	TSubclassOf<class UDGDigicamWidget> WidgetClass;

	// 소낙의 집(주점) 레벨 이름. 에디터에서 직접 입력.
	UPROPERTY(EditAnywhere, Category = "Digicam|Settings")
	FName BaseLevelName; 

	// 현재 레벨이 주점(여기)인지 확인하는 헬퍼 함수
	bool IsAtBaseLevel() const;

	// 생성된 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<class UDGDigicamWidget> DigicamWidget;

	// 매칭된 현재 데이터
	FSpacetimeData CurrentMatchedData;
};