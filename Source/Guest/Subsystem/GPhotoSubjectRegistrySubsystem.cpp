// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GPhotoSubjectRegistrySubsystem.h"
#include "Guest/Components/GPhotoSubjectComponent.h"

void UGPhotoSubjectRegistrySubsystem::RegisterSubject(UGPhotoSubjectComponent* Subject)
{
	if (!Subject)
	{
		return;
	}

	RegisteredSubjects.Add(Subject);
}

void UGPhotoSubjectRegistrySubsystem::UnregisterSubject(UGPhotoSubjectComponent* Subject)
{
	if (!Subject)
	{
		return;
	}

	RegisteredSubjects.Remove(Subject);
}

void UGPhotoSubjectRegistrySubsystem::GetSubjects(TArray<UGPhotoSubjectComponent*>& OutSubjects)
{
	OutSubjects.Reset();

	for (auto It = RegisteredSubjects.CreateIterator(); It; ++It)
	{
		UGPhotoSubjectComponent* Subject = It->Get();
		if (!Subject)
		{
			It.RemoveCurrent();
			continue;
		}

		OutSubjects.Add(Subject);
	}
}
