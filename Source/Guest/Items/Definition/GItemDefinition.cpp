#include "GItemDefinition.h"

FPrimaryAssetId UGItemDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetGuestItemAssetType(), GetFName());
}

FPrimaryAssetType UGItemDefinition::GetGuestItemAssetType()
{
	return FPrimaryAssetType("GuestItem");
}
