// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GDigicamComponent.h"

#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "Guest/Subsystem/GSpacetimeSubsystem.h"
#include "Guest/Components/GCameraComponent.h"
#include "Guest/Utils/GLog.h"

UGDigicamComponent::UGDigicamComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentState = EDigicamState::Inactive;
	SelectedYear = 2010; // 소낙이 도망친 기본 연도
	SelectedAreaCode = 0;
}

void UGDigicamComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGDigicamComponent::ActivateDigicam()
{
	if (CurrentState == EDigicamState::Inactive)
	{
		CurrentState = EDigicamState::TimeSetting;
		G_LOG(TEXT("디카 활성화: 수거 준비"));
	}

	UpdateSearch();
	BroadcastSearchState();
}

void UGDigicamComponent::DeactivateDigicam()
{
	CurrentState = EDigicamState::Inactive;
	G_LOG(TEXT("디카 비활성화: 귀가 상태 유지"));
}

void UGDigicamComponent::HandleVerticalInput(float Value)
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
	else if (CurrentState == EDigicamState::LocationFocus || CurrentState == EDigicamState::ReadyToSnap)
	{
		SelectedAreaCode += (Value > 0) ? 1 : -1;
		G_LOG(TEXT("구역 코드 조절: %d"), SelectedAreaCode);
	}

	UpdateSearch();
}

void UGDigicamComponent::HandleHorizontalInput(float Value)
{
	if (Value == 0.0f) return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastHorizontalInputTime < InputDelay) return;
	
	LastHorizontalInputTime = CurrentTime;

	if (CurrentState == EDigicamState::TimeSetting && Value > 0)
	{
		CurrentState = EDigicamState::LocationFocus;
		G_LOG(TEXT("모드 전환: 장소 설정"));
		BroadcastSearchState();
	}
	else if ((CurrentState == EDigicamState::LocationFocus || CurrentState == EDigicamState::ReadyToSnap) && Value < 0)
	{
		CurrentState = EDigicamState::TimeSetting;
		G_LOG(TEXT("모드 전환: 연도 설정"));
		BroadcastSearchState();
	}
}

void UGDigicamComponent::HandleShutter()
{
	UGSpacetimeSubsystem* SpacetimeSS = GetWorld()->GetGameInstance()->GetSubsystem<UGSpacetimeSubsystem>();

	if (CurrentState != EDigicamState::ReadyToSnap || (SpacetimeSS && SpacetimeSS->IsTravelInProgress()))
	{
		G_WARN(TEXT("좌표 불일치 또는 이동 중 — 이동 불가"));
		OnShutterDenied.Broadcast(LastSearchResult);
		return;
	}

	// 이동 전 현재 장면 촬영
	if (AActor* Owner = GetOwner())
	{
		if (UGCameraComponent* CamComp = Owner->FindComponentByClass<UGCameraComponent>())
		{
			FPhotoData Meta;
			Meta.InGameYear  = CurrentMatchedData.TargetYear;
			Meta.PlaceName   = CurrentMatchedData.PlaceName;
			Meta.AreaCode    = CurrentMatchedData.AreaCode;
			Meta.StoryDate   = CurrentMatchedData.StoryDate;
			CamComp->TakePhoto(Meta);
		}
	}

	if (!SpacetimeSS) return;

	G_LOG(TEXT("시공간 이동: %s"), *CurrentMatchedData.PlaceName.ToString());
	SpacetimeSS->ExecuteTravel(CurrentMatchedData);
}

void UGDigicamComponent::UpdateSearch()
{
	if (!GetWorld() || !GetWorld()->GetGameInstance()) return;

	// GetWorld()를 거쳐서 서브시스템에 접근
	UGSpacetimeSubsystem* SpacetimeSS = GetWorld()->GetGameInstance()->GetSubsystem<UGSpacetimeSubsystem>();
	if (SpacetimeSS)
	{
		// 서브시스템에 검색 위임 및 결과 저장
		LastSearchResult = SpacetimeSS->SearchSpacetime(SelectedYear, SelectedAreaCode, CurrentMatchedData);

		if (LastSearchResult == ESpacetimeSearchResult::Found)
		{
			CurrentState = EDigicamState::ReadyToSnap;
		}
		else if (CurrentState == EDigicamState::ReadyToSnap)
		{
			CurrentState = EDigicamState::LocationFocus;
		}

		BroadcastSearchState();
	}
}

void UGDigicamComponent::BroadcastSearchState()
{
	OnDigicamSearchUpdated.Broadcast(SelectedYear, SelectedAreaCode, CurrentMatchedData, CurrentState, LastSearchResult);
}

