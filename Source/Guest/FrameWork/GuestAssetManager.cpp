// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GuestAssetManager.h"

#include "Guest/Items/Definition/GItemDefinition.h"


UGuestAssetManager& UGuestAssetManager::Get()
{
	UGuestAssetManager* Singleton = Cast<UGuestAssetManager>(GEngine->AssetManager.Get());
	if (Singleton)
	{
		return *Singleton;
	}
	
	UE_LOG(LogLoad, Fatal, TEXT("게스트 에셋 매니저 필요"));
	
	return(*NewObject<UGuestAssetManager>());
}

void UGuestAssetManager::LoadGuestItems(const FStreamableDelegate& LoadFinishedDelegate)
{
	LoadPrimaryAssetsWithType(UGItemDefinition::GetGuestItemAssetType(), TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UGuestAssetManager::GuestItemLoadFinished, LoadFinishedDelegate));
}

bool UGuestAssetManager::GetLoadedItems(TArray<const UGItemDefinition*>& OutItems)
{
	TArray<UObject*> LoadedObjects;
	bool bLoaded = GetPrimaryAssetObjectList(UGItemDefinition::GetGuestItemAssetType(), LoadedObjects);
	
	if (bLoaded)
	{
		for (UObject* LoadedObject : LoadedObjects)
		{
			OutItems.Add(Cast<UGItemDefinition>(LoadedObject));
		}
	}
	
	return bLoaded;
}

void UGuestAssetManager::GuestItemLoadFinished(FStreamableDelegate CallBack)
{
	CallBack.ExecuteIfBound();
}
