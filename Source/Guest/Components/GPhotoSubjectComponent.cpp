// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GPhotoSubjectComponent.h"
#include "Guest/Subsystem/GPhotoSubjectRegistrySubsystem.h"
#include "Guest/Utils/GLog.h"

UGPhotoSubjectComponent::UGPhotoSubjectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGPhotoSubjectComponent::GetWorldBox(FVector& OutCenter, FVector& OutExtent) const
{
	if (bUseOwnerBounds && GetOwner())
	{
		GetOwner()->GetActorBounds(false, OutCenter, OutExtent);
		return;
	}

	OutCenter = GetComponentLocation();

	// 스케일을 곱해두면 에디터에서 컴포넌트를 늘리는 것만으로 판정 범위를 맞출 수 있다.
	OutExtent = BoxExtent * GetComponentScale();
}

void UGPhotoSubjectComponent::BeginPlay()
{
	Super::BeginPlay();

	if (SubjectID.IsNone())
	{
		// 등록해봐야 판정 결과가 NAME_None이라 목표를 진행시키지 못한다.
		// 조용히 넘어가면 "왜 퀘스트가 안 되지"로 돌아오므로 여기서 알린다.
		G_WARN(TEXT("피사체: SubjectID가 비어 있어 등록하지 않습니다. (%s)"),
			*GetReadableName());
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (UGPhotoSubjectRegistrySubsystem* Registry = World->GetSubsystem<UGPhotoSubjectRegistrySubsystem>())
		{
			Registry->RegisterSubject(this);
		}
	}
}

void UGPhotoSubjectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UGPhotoSubjectRegistrySubsystem* Registry = World->GetSubsystem<UGPhotoSubjectRegistrySubsystem>())
		{
			Registry->UnregisterSubject(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}
