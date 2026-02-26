// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "DGDigicamComponent.h"

#include "Blueprint/UserWidget.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "Guest/Subsystem/GSpacetimeSubsystem.h"
#include "Guest/Utils/GLog.h"
#include "Guest/UI/DGDigicamWidget.h"
#include "Kismet/GameplayStatics.h"

UDGDigicamComponent::UDGDigicamComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentState = EDigicamState::Inactive;
	SelectedYear = 2010; // 소낙이 도망친 기본 연도
	SelectedAreaCode = 0;
}

void UDGDigicamComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDGDigicamComponent::ActivateDigicam()
{
	if (CurrentState == EDigicamState::Inactive)
	{
		CurrentState = EDigicamState::TimeSetting;
		G_LOG(TEXT("디카 활성화: 수거 준비"));
	}

	if (WidgetClass && !DigicamWidget)
	{
		DigicamWidget = CreateWidget<UDGDigicamWidget>(GetWorld(), WidgetClass);
		if (DigicamWidget) DigicamWidget->AddToViewport();
	}

	UpdateSearch();
}

void UDGDigicamComponent::DeactivateDigicam()
{
	CurrentState = EDigicamState::Inactive;
	G_LOG(TEXT("디카 비활성화: 귀가 상태 유지"));
}

void UDGDigicamComponent::HandleVerticalInput(float Value)
{
	if (Value == 0.0f) return;

	// 시간 체크: 현재 시간 - 마지막 입력 시간이 지연 시간보다 작으면 무시
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastVerticalInputTime < InputDelay) return;

	// 입력 허용 시 시간 갱신
	LastVerticalInputTime = CurrentTime;

	if (CurrentState == EDigicamState::TimeSetting)
	{
		SelectedYear += (Value > 0) ? 1 : -1;
		G_LOG(TEXT("연도 조절: %d"), SelectedYear);
	}
	else if (CurrentState == EDigicamState::LocationFocus)
	{
		SelectedAreaCode += (Value > 0) ? 1 : -1;
		G_LOG(TEXT("구역 코드 조절: %d"), SelectedAreaCode);
	}

	UpdateSearch();
}

void UDGDigicamComponent::HandleHorizontalInput(float Value)
{
	if (Value == 0.0f) return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastHorizontalInputTime < InputDelay) return;
	
	LastHorizontalInputTime = CurrentTime;

	if (CurrentState == EDigicamState::TimeSetting && Value > 0)
	{
		CurrentState = EDigicamState::LocationFocus;
		G_LOG(TEXT("모드 전환: 장소 설정"));
	}
	else if (CurrentState == EDigicamState::LocationFocus && Value < 0)
	{
		CurrentState = EDigicamState::TimeSetting;
		G_LOG(TEXT("모드 전환: 연도 설정"));
	}
}

void UDGDigicamComponent::HandleShutter()
{
	if (IsAtBaseLevel())
	{
		// CurrentMatchedData가 선언되어야 아래 로직이 컴파일됨
		if (CurrentState == EDigicamState::ReadyToSnap)
		{
			G_LOG(TEXT("'저기'로 수거를 시작합니다: %s"), *CurrentMatchedData.PlaceName.ToString());
			UGameplayStatics::OpenLevel(GetWorld(), CurrentMatchedData.LevelName);
		}
		else
		{
			G_WARN(TEXT("좌표가 일치하지 않아 수거를 시작할 수 없습니다."));
		}
	}
	else
	{
		if (!BaseLevelName.IsNone())
		{
			G_LOG(TEXT("수거를 마치고 '여기'로 귀가합니다."));
			UGameplayStatics::OpenLevel(GetWorld(), BaseLevelName);
		}
	}
}

void UDGDigicamComponent::UpdateSearch()
{
	if (!GetWorld() || !GetWorld()->GetGameInstance()) return;

	// GetWorld()를 거쳐서 서브시스템에 접근
	UGSpacetimeSubsystem* SpacetimeSS = GetWorld()->GetGameInstance()->GetSubsystem<UGSpacetimeSubsystem>();
	if (SpacetimeSS)
	{
		// 서브시스템에 검색 위임 및 결과 저장
		bool bFound = SpacetimeSS->SearchSpacetime(SelectedYear, SelectedAreaCode, CurrentMatchedData);

		if (bFound)
		{
			CurrentState = EDigicamState::ReadyToSnap;
		}
		else if (CurrentState == EDigicamState::ReadyToSnap)
		{
			CurrentState = EDigicamState::LocationFocus;
		}

		if (DigicamWidget)
		{
			DigicamWidget->UpdateLCD(CurrentState, SelectedYear, SelectedAreaCode, CurrentMatchedData);
		}
	}
}

bool UDGDigicamComponent::IsAtBaseLevel() const
{
	if (!GetWorld()) return false;

	// 현재 맵의 이름을 가져옴 (에디터 실행 시 붙는 접두사 제거 포함)
	FString CurrentMapName = GetWorld()->GetMapName();
	CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	return CurrentMapName == BaseLevelName.ToString();
}
