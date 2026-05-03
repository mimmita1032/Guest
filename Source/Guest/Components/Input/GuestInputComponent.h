// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Guest/Data/Input/GInputConfigData.h"
#include "GuestInputComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUEST_API UGuestInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserObject, typename CallbackFunc>
	void BindAbilityInputAction(const UGInputConfigData* InInputConfigData, UserObject* ContextObject, CallbackFunc PressedFunc, CallbackFunc ReleasedFunc)
	{
		checkf(InInputConfigData, TEXT("인풋 구성 데이터 에셋이 할당 안됨"));
		
		for (const FGuestInputConfig& AbilityInputConfig : InInputConfigData->AbilityInputActions)
		{
			if (!AbilityInputConfig.InputAction || !AbilityInputConfig.InputTag.IsValid()) continue;
			
			BindAction(AbilityInputConfig.InputAction, ETriggerEvent::Started, ContextObject, PressedFunc, AbilityInputConfig.InputTag);
			BindAction(AbilityInputConfig.InputAction, ETriggerEvent::Started, ContextObject, ReleasedFunc, AbilityInputConfig.InputTag);
		}
	}
};
