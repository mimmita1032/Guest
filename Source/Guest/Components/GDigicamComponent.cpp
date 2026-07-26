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
	// 셔터는 오직 촬영만 한다. 이동은 HandleTravel의 몫이다.
	// 찍히는 것은 "지금 눈앞의 장면"이고, 사진에 박히는 장소·연도도 "지금 있는 좌표"다.
	if (CurrentState == EDigicamState::Inactive)
	{
		// 디지캠을 꺼내지 않았으면 찍을 카메라가 없다
		return;
	}

	AActor* Owner = GetOwner();
	UGCameraComponent* CamComp = Owner ? Owner->FindComponentByClass<UGCameraComponent>() : nullptr;
	if (!CamComp)
	{
		G_WARN(TEXT("디지캠: 카메라 컴포넌트를 찾을 수 없어 촬영할 수 없습니다."));
		return;
	}

	UGSpacetimeSubsystem* SpacetimeSS = GetWorld()->GetGameInstance()->GetSubsystem<UGSpacetimeSubsystem>();

	// 이동 페이드 중에는 화면이 이미 걷히는 중이라 의미 있는 사진이 나오지 않는다
	if (SpacetimeSS && SpacetimeSS->IsTravelInProgress())
	{
		return;
	}

	// 시작 레벨처럼 PostLoadMap 훅을 놓쳤을 수 있는 경우를 위해 여기서 한 번 더 확정을 시도한다
	if (SpacetimeSS)
	{
		SpacetimeSS->EnsureCurrentLocation();
	}

	FPhotoData Meta;
	if (SpacetimeSS && SpacetimeSS->HasCurrentLocation())
	{
		const FSpacetimeData Here = SpacetimeSS->GetCurrentLocation();
		Meta.InGameYear = Here.TargetYear;
		Meta.PlaceName  = Here.PlaceName;
		Meta.AreaCode   = Here.AreaCode;
		Meta.StoryDate  = Here.StoryDate;
		Meta.SubjectID  = Here.PhotoSubjectID;
	}
	else
	{
		// 좌표 미확정 — 기본값을 베껴 넣으면 사진에 엉뚱한 연도(FSpacetimeData의 기본값 2010)가
		// 박힌다. 차라리 비워 두는 편이 정직하다.
		G_WARN(TEXT("디지캠: 현재 좌표가 확정되지 않아 사진에 장소 정보를 남기지 못합니다. "
			"이 레벨의 행이 DT_SpacetimeData에 있는지 확인하세요."));
	}

	if (!CamComp->TakePhoto(Meta))
	{
		G_WARN(TEXT("디지캠: 사진을 남기지 못했습니다 (인벤토리 공간 부족 등)."));
	}
}

void UGDigicamComponent::HandleTravel()
{
	UGSpacetimeSubsystem* SpacetimeSS = GetWorld()->GetGameInstance()->GetSubsystem<UGSpacetimeSubsystem>();

	if (CurrentState != EDigicamState::ReadyToSnap || (SpacetimeSS && SpacetimeSS->IsTravelInProgress()))
	{
		G_WARN(TEXT("좌표 불일치 또는 이동 중 — 이동 불가"));
		OnShutterDenied.Broadcast(LastSearchResult);
		return;
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

