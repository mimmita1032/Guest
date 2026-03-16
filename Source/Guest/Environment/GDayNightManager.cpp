// Fill out your copyright notice in the Description page of Project Settings.

#include "GDayNightManager.h"
#include "Engine/DirectionalLight.h"
#include "Misc/DateTime.h"

ADGDayNightManager::ADGDayNightManager()
{
	// 매 프레임마다 태양 각도를 미세하게 돌려야 하므로 Tick을 켭니다.
	PrimaryActorTick.bCanEverTick = true;
}

void ADGDayNightManager::BeginPlay()
{
	Super::BeginPlay();
}

void ADGDayNightManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 매 프레임 시간 동기화 함수 실행
	UpdateRealTimeSky();
}

void ADGDayNightManager::UpdateRealTimeSky()
{
	// 에디터에서 태양을 연결 안 해뒀으면 크래시 나지 않게 방어
	if (!SunLight) return;

	//유저 컴퓨터의 현재 현실 시간을 그대로 
	FDateTime CurrentRealTime = FDateTime::Now();
	
	int32 Hour = CurrentRealTime.GetHour();
	int32 Minute = CurrentRealTime.GetMinute();
	int32 Second = CurrentRealTime.GetSecond(); // 초 단위까지 가져와야 해가 안끊기는듯?
	
	//시간을 소수점으로 변환
	float TimeOfDay = Hour + (Minute / 60.0f) + (Second / 3600.0f);
	
	//24시간을 360도로 변환 (Pitch 각도)
	float SunPitch = (TimeOfDay / 24.0f) * 360.0f;
	
	//언리얼 기본 하늘 템플릿 기준, 밤 12시에 태양이 바닥을 보게 하려면 오프셋 필요
	//우선 +90.0f를 하고 디렉셔널 라이트랑 비교 필요.,..
	SunPitch += 90.0f; 

	//계산된 각도로 태양(Directional Light)을 회전시킵니다.
	SunLight->SetActorRotation(FRotator(SunPitch, 0.0f, 0.0f));
}