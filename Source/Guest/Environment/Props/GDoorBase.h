// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "Guest/Interfaces/GTimeListenerInterface.h"
#include "GameplayTagContainer.h"
#include "Components/TimelineComponent.h"
#include "GDoorBase.generated.h"

UENUM(BlueprintType)
enum class EDoorType : uint8
{
	Hinge			UMETA(DisplayName = "일반 여닫이문"),
	DoubleHinge		UMETA(DisplayName = "양쪽 여닫이문"),
	Sliding			UMETA(DisplayName = "미닫이문"),
	Vertical		UMETA(DisplayName = "수직 셔터/해치")
};

UCLASS()
class GUEST_API AGDoorBase : public AActor, public IGInteractableInterface, public IGTimeListenerInterface
{
	GENERATED_BODY()
	
public:	
	AGDoorBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

#pragma region Components
	// 문 프레임(틀)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
	TObjectPtr<class UStaticMeshComponent> DoorFrame;

	// 왼쪽(기본) 문
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh_Left;

	// 오른쪽 문 (DoubleHinge 전용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh_Right;

	// 부드러운 개폐를 위한 타임라인
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
	TObjectPtr<UTimelineComponent> DoorTimeline;
#pragma endregion

#pragma region Settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Settings")
	EDoorType DoorType = EDoorType::Hinge;

	// 에디터에서 설정할 애니메이션 커브 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Settings")
	TObjectPtr<class UCurveFloat> DoorCurve;

	// 회전형 문의 열림 각도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Settings")
	float RotateAngle = 90.0f;

	// 이동형(미닫이/수직) 문의 열림 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Settings")
	float SlideDistance = 100.0f;
#pragma endregion

#pragma region State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|State")
	bool bIsOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Lock")
	bool bIsLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Lock")
	FGameplayTag RequiredKeyTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Time")
	float OpenHour = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Time")
	float CloseHour = 22.0f;
#pragma endregion

public:	
	virtual void Interact(AActor* Interactor) override;
	virtual FText GetInteractText() const override; // UI 텍스트 반환
	virtual void OnTimeChanged(float CurrentHour) override;

protected:
	void OpenDoor();
	void CloseDoor();

	// 타임라인 실행 시 매 프레임 호출될 함수
	UFUNCTION()
	void OnDoorTimelineUpdate(float Value);

private:
	// 문의 원래 위치와 회전값을 기억하기 위한 변수
	FRotator InitialRotation_Left;
	FRotator InitialRotation_Right;
	FVector InitialLocation_Left;
	FVector InitialLocation_Right;
};