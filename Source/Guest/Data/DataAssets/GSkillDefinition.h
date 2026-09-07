// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Guest/GuestTypes/GuestGameplayTypes.h"
#include "GSkillDefinition.generated.h"

/*=================
전문성(Skill) 설계도.
CurrentState/CurrentProgress 같은 플레이어별 런타임 상태는 여기 두지 않고
향후 UGSkillMasterySubsystem에서 관리한다.
=================*/
UCLASS()
class GUEST_API UGSkillDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetGuestSkillAssetType();

	// 게임 로직 식별자. AssetManager용 PrimaryAssetId(GetFName())와는 별개다.
	UPROPERTY(EditDefaultsOnly, Category = "Default", meta = (Categories = "Guest.Skill"))
	FGameplayTag SkillTag;

	UPROPERTY(EditDefaultsOnly, Category = "Default")
	FText SkillName;

	UPROPERTY(EditDefaultsOnly, Category = "Default")
	FText Description;

	UPROPERTY(EditDefaultsOnly, Category = "Default")
	int32 Tier = 0;

	// 선행 Skill들. 기본 정책은 전부 Mastered일 때 충족.
	UPROPERTY(EditDefaultsOnly, Category = "Default", meta = (Categories = "Guest.Skill"))
	TArray<FGameplayTag> RequiredSkills;

	// UGItemDefinition::ItemID와 같은 FName 체계를 사용한다.
	UPROPERTY(EditDefaultsOnly, Category = "Default")
	FName RequiredBookID;

	UPROPERTY(EditDefaultsOnly, Category = "Narrative")
	FText DiscoveryLocationHint;

	UPROPERTY(EditDefaultsOnly, Category = "Narrative")
	FText MasteryDescription;

	UPROPERTY(EditDefaultsOnly, Category = "Mastery")
	TArray<FSkillMasteryCondition> MasteryConditions;

	UPROPERTY(EditDefaultsOnly, Category = "Mastery")
	FSkillReward Reward;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};