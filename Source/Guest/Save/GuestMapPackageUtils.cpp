#include "Guest/Save/GuestMapPackageUtils.h"

static bool GuestTryStripPIELeaf(const FString& Leaf, FString& OutCleanLeaf)
{
	static const FString Prefix(TEXT("UEDPIE_"));
	if (!Leaf.StartsWith(Prefix, ESearchCase::CaseSensitive))
	{
		return false;
	}
	int32 Idx = Prefix.Len();
	while (Idx < Leaf.Len() && FChar::IsDigit(Leaf[Idx]))
	{
		++Idx;
	}
	if (Idx >= Leaf.Len() || Leaf[Idx] != TEXT('_'))
	{
		return false;
	}
	OutCleanLeaf = Leaf.Mid(Idx + 1);
	return !OutCleanLeaf.IsEmpty();
}
namespace GuestMapPackage
{
	FString StripPIEFromPackagePath(const FString& PackagePath)
	{
		if (PackagePath.IsEmpty())
		{
			return PackagePath;
		}
		int32 LastSlash = INDEX_NONE;
		if (!PackagePath.FindLastChar(TEXT('/'), LastSlash))
		{
			FString Clean;
			if (GuestTryStripPIELeaf(PackagePath, Clean))
			{
				return Clean;
			}
			return PackagePath;
		}
		const FString Prefix = PackagePath.Left(LastSlash + 1);
		const FString Leaf = PackagePath.Mid(LastSlash + 1);
		FString CleanLeaf;
		if (GuestTryStripPIELeaf(Leaf, CleanLeaf))
		{
			return Prefix + CleanLeaf;
		}
		return PackagePath;
	}
}