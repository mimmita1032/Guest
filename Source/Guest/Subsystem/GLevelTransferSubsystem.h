// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GLevelTransferSubsystem.generated.h"

UCLASS()
class GUEST_API UGLevelTransferSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Level Transfer")
	void SetTargetSpawnTag(FName InTag);

	UFUNCTION(BlueprintPure, Category = "Level Transfer")
	FName GetTargetSpawnTag() const { return TargetSpawnTag; }

	UFUNCTION(BlueprintCallable, Category = "Level Transfer")
	void SetTransferTime(float InTime);

	UFUNCTION(BlueprintPure, Category = "Level Transfer")
	float GetTransferTime() const { return StoredTime; }

	UFUNCTION(BlueprintCallable, Category = "Level Transfer")
	void ClearTransferData();

private:
	UPROPERTY()
	FName TargetSpawnTag = NAME_None;

	UPROPERTY()
	float StoredTime = -1.0f;
};