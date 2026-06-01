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

	UE_LOG(LogLoad, Error,
		TEXT("UGuestAssetManager가 아닙니다. DefaultEngine.ini [/Script/Engine.Engine] AssetManagerClassName=/Script/Guest.GuestAssetManager 확인 후 에디터 재시작."));

	// Import 등은 Asset Registry fallback 사용. 여기 도달 시 비상용 인스턴스 반환 (Fatal 금지).
	static TObjectPtr<UGuestAssetManager> EmergencyFallback = NewObject<UGuestAssetManager>();
	return *EmergencyFallback.Get();
}

void UGuestAssetManager::LoadGuestItems(const FStreamableDelegate& LoadFinishedDelegate)
{
	LoadPrimaryAssetsWithType(UGItemDefinition::GetGuestItemAssetType(), TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UGuestAssetManager::GuestItemLoadFinished, LoadFinishedDelegate));
}

bool UGuestAssetManager::GetLoadedItems(TArray<const UGItemDefinition*>& OutItems) const
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
