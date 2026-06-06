// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GPhotoLibrarySubsystem.h"

void UGPhotoLibrarySubsystem::AddPhoto(const FPhotoData& Photo)
{
	Photos.Add(Photo);
	OnPhotoAdded.Broadcast(Photo);
}
