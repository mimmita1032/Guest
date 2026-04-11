// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Data/DataAssets/GNPCScheduleDataAsset.h"

const FNPCScheduleEntry* UGNPCScheduleDataAsset::FindActiveEntry(float CurrentHour) const
{
	for (const FNPCScheduleEntry& Entry : Schedule)
	{
		const float Start = Entry.GetStartAsFloat();
		const float End   = Entry.GetEndAsFloat();

		// 자정을 넘기는 구간 (예: 23:00 ~ 06:00)
		const bool bActive = (Start > End)
			? (CurrentHour >= Start || CurrentHour < End)
			: (CurrentHour >= Start && CurrentHour < End);

		if (bActive)
		{
			return &Entry;
		}
	}
	return nullptr;
}
