// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Core/Controllers/GuestPlayerController.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/UI/Settings/GuestUISettings.h"
#include "Guest/UI/Layout/GuestPrimaryLayout.h"
#include "Blueprint/UserWidget.h"

AGuestPlayerController::AGuestPlayerController()
{
}

void AGuestPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 플레이어인 경우에만 UI 레이아웃 생성
	if (IsLocalController())
	{
		CreatePrimaryLayout();
	}
}

void AGuestPlayerController::CreatePrimaryLayout()
{
	// 1. UI 설정 로드
	const UGuestUISettings* UISettings = GetDefault<UGuestUISettings>();
	if (!UISettings) return;

	// 2. Primary Layout 클래스 소프트 레퍼런스 확인
	TSoftClassPtr<UUserWidget> LayoutClassPtr = UISettings->PrimaryLayoutClass;
	if (LayoutClassPtr.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("[GuestUI] GuestUISettings에 PrimaryLayoutClass가 설정되지 않았습니다!"));
		return;
	}

	// 3. 클래스 동기 로드 (최상위 레이아웃은 게임 시작 시 즉시 필요하므로 동기 로드)
	UClass* LayoutClass = LayoutClassPtr.LoadSynchronous();
	if (!LayoutClass) return;

	// 4. 위젯 생성 및 뷰포트 추가
	PrimaryLayout = CreateWidget<UGuestPrimaryLayout>(this, LayoutClass);
	if (PrimaryLayout)
	{
		PrimaryLayout->AddToViewport(0); // 가장 낮은 ZOrder로 시작 (내부 스택들이 그 위를 덮음)
		
		// 참고: PrimaryLayout->NativeOnInitialized() 내부에서 
		// 각 Stack들을 Subsystem에 RegisterStack하게 됩니다.
		UE_LOG(LogTemp, Log, TEXT("[GuestUI] Primary Layout 생성 및 뷰포트 추가 완료."));
	}
}

UGuestUISubsystem* AGuestPlayerController::GetUISubsystem() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		return GI->GetSubsystem<UGuestUISubsystem>();
	}
	return nullptr;
}