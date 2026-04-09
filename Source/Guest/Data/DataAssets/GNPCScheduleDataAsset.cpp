// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Data/DataAssets/GNPCScheduleDataAsset.h"

const FNPCScheduleEntry* UGNPCScheduleDataAsset::FindActiveEntry(float CurrentHour) const
{
	for (const FNPCScheduleEntry& Entry : Schedule)
	{
		if (CurrentHour >= Entry.GetStartAsFloat() && CurrentHour < Entry.GetEndAsFloat())
		{
			return &Entry;
		}
	}
	return nullptr;
}
