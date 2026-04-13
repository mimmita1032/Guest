#include "Guest/Save/GuestSaveSlotNames.h"

namespace GuestSaveSlots
{
	int32 DefaultUserIndex()
	{
		return 0;
	}
	FString MakeSlotName(int32 SlotIndex)
	{
		return FString::Printf(TEXT("GuestSave_%d"), SlotIndex);
	}
}
