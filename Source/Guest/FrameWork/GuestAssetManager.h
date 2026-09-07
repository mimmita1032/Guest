// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GuestAssetManager.generated.h"

class UGItemDefinition;
class UGSkillDefinition;
/**
 *
 */
UCLASS()
class GUEST_API UGuestAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UGuestAssetManager& Get();
	void LoadGuestItems(const FStreamableDelegate& LoadFinishedDelegate);
	bool GetLoadedItems(TArray<const UGItemDefinition*>& OutItems) const;

	void LoadGuestSkills(const FStreamableDelegate& LoadFinishedDelegate);
	bool GetLoadedSkills(TArray<const UGSkillDefinition*>& OutSkills) const;
private:
	void GuestItemLoadFinished(FStreamableDelegate CallBack);
	void GuestSkillLoadFinished(FStreamableDelegate CallBack);
};
