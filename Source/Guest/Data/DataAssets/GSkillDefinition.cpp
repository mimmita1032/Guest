// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GSkillDefinition.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

FPrimaryAssetId UGSkillDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetGuestSkillAssetType(), GetFName());
}

FPrimaryAssetType UGSkillDefinition::GetGuestSkillAssetType()
{
	return FPrimaryAssetType("GuestSkill");
}

#if WITH_EDITOR
EDataValidationResult UGSkillDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	TSet<FGameplayTag> SeenProgressEventTags;
	for (const FSkillMasteryCondition& Condition : MasteryConditions)
	{
		if (!Condition.ProgressEventTag.IsValid())
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			Context.AddError(FText::FromString(TEXT("MasteryConditions에 ProgressEventTag가 비어 있는 항목이 있습니다.")));
			continue;
		}

		bool bAlreadySeen = false;
		SeenProgressEventTags.Add(Condition.ProgressEventTag, &bAlreadySeen);
		if (bAlreadySeen)
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
			Context.AddError(FText::FromString(FString::Printf(TEXT("MasteryConditions에 동일한 ProgressEventTag(%s)가 중복됩니다."), *Condition.ProgressEventTag.ToString())));
		}
	}

	return Result;
}
#endif