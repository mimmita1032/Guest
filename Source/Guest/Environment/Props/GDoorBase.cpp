// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GDoorBase.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Guest/Utils/GLog.h"

AGDoorBase::AGDoorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	RootComponent = DoorFrame;

	DoorMesh_Left = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh_Left"));
	DoorMesh_Left->SetupAttachment(RootComponent);

	DoorMesh_Right = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh_Right"));
	DoorMesh_Right->SetupAttachment(RootComponent);

	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeline"));
}

void AGDoorBase::BeginPlay()
{
	Super::BeginPlay();

	// 초기 트랜스폼 저장 (나중에 닫힐 때 원래 자리로 돌아오려고)
	InitialRotation_Left = DoorMesh_Left->GetRelativeRotation();
	InitialRotation_Right = DoorMesh_Right->GetRelativeRotation();
	InitialLocation_Left = DoorMesh_Left->GetRelativeLocation();
	InitialLocation_Right = DoorMesh_Right->GetRelativeLocation();

	// 타임라인 커브가 할당되어 있다면 바인딩
	if (DoorCurve)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("OnDoorTimelineUpdate"));
		DoorTimeline->AddInterpFloat(DoorCurve, TimelineProgress);
	}
}

void AGDoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGDoorBase::Interact(AActor* Interactor)
{
	if (bIsLocked)
	{
		UE_LOG(LogTemp, Warning, TEXT("문 조작 실패: 잠겨있음"));
		return;
	}

	bIsOpen = !bIsOpen;
	
	if (bIsOpen)
	{
		OpenDoor();
	}
	else
	{
		CloseDoor();
	}
}

FText AGDoorBase::GetInteractText() const
{
	if (bIsLocked) return FText::FromString(TEXT("잠김"));
	return bIsOpen ? FText::FromString(TEXT("문 닫기")) : FText::FromString(TEXT("문 열기"));
}

void AGDoorBase::OnTimeChanged(float CurrentHour)
{
	if (CurrentHour >= OpenHour && CurrentHour < CloseHour)
	{
		bIsLocked = false;
	}
	else
	{
		bIsLocked = true;
	}
}

void AGDoorBase::OpenDoor()
{
	UE_LOG(LogTemp, Log, TEXT("문 개방 시작"));
	if (DoorCurve) DoorTimeline->Play();
}

void AGDoorBase::CloseDoor()
{
	UE_LOG(LogTemp, Log, TEXT("문 폐쇄 시작"));
	if (DoorCurve) DoorTimeline->Reverse(); // 타임라인 역재생
}

void AGDoorBase::OnDoorTimelineUpdate(float Value)
{
	// Value는 커브에 따라 0.0에서 1.0 사이로 변함
	
	switch (DoorType)
	{
	case EDoorType::Hinge:
		{
			// 한 쪽 문 90도 회전
			FRotator TargetRot = InitialRotation_Left + FRotator(0.0f, RotateAngle, 0.0f);
			DoorMesh_Left->SetRelativeRotation(FMath::Lerp(InitialRotation_Left, TargetRot, Value));
			break;
		}
	case EDoorType::DoubleHinge:
		{
			// 양쪽 문 반대 방향으로 회전
			FRotator TargetRotLeft = InitialRotation_Left + FRotator(0.0f, RotateAngle, 0.0f);
			FRotator TargetRotRight = InitialRotation_Right + FRotator(0.0f, -RotateAngle, 0.0f);
			DoorMesh_Left->SetRelativeRotation(FMath::Lerp(InitialRotation_Left, TargetRotLeft, Value));
			DoorMesh_Right->SetRelativeRotation(FMath::Lerp(InitialRotation_Right, TargetRotRight, Value));
			break;
		}
	case EDoorType::Sliding:
		{
			// 좌우(Y축)로 슬라이드
			FVector TargetLoc = InitialLocation_Left + FVector(0.0f, SlideDistance, 0.0f);
			DoorMesh_Left->SetRelativeLocation(FMath::Lerp(InitialLocation_Left, TargetLoc, Value));
			break;
		}
	case EDoorType::Vertical:
		{
			// 위아래(Z축)로 상승
			FVector TargetLoc = InitialLocation_Left + FVector(0.0f, 0.0f, SlideDistance);
			DoorMesh_Left->SetRelativeLocation(FMath::Lerp(InitialLocation_Left, TargetLoc, Value));
			break;
		}
	}
}