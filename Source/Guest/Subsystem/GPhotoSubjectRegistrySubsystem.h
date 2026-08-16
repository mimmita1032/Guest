// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GPhotoSubjectRegistrySubsystem.generated.h"

class UGPhotoSubjectComponent;

/**
 * 현재 월드에 살아 있는 피사체 컴포넌트 등록소.
 *
 * 셔터를 누를 때마다 TActorIterator로 월드를 훑으면 촬영 한 번의 비용이 레벨 크기에
 * 비례해 커진다. 촬영은 프레임 중간에 픽셀 읽기와 PNG 압축까지 하는 무거운 경로라
 * 여기에 스캔까지 얹고 싶지 않다. 그래서 컴포넌트가 BeginPlay에 자기등록한다.
 *
 * 레벨마다 목록이 갈려야 하므로 GameInstance가 아니라 WorldSubsystem이다.
 */
UCLASS()
class GUEST_API UGPhotoSubjectRegistrySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// UGPhotoSubjectComponent::BeginPlay에서 호출한다. 중복 등록은 안전하게 무시된다.
	void RegisterSubject(UGPhotoSubjectComponent* Subject);

	// UGPhotoSubjectComponent::EndPlay에서 호출한다.
	void UnregisterSubject(UGPhotoSubjectComponent* Subject);

	// 살아 있는 등록분만 돌려준다. 호출할 때 무효해진 항목을 함께 정리한다 —
	// Tick을 돌리지 않기 위해서다.
	void GetSubjects(TArray<UGPhotoSubjectComponent*>& OutSubjects);

private:
	// 등록소가 피사체의 수명을 붙잡으면 안 되므로 약참조로 들고 있는다.
	TSet<TWeakObjectPtr<UGPhotoSubjectComponent>> RegisteredSubjects;
};
