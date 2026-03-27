// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GTimeListenerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UGTimeListenerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *  시간에 따라 상태가 변해야하는 액터들을 위한 인터페이스
 */
class GUEST_API IGTimeListenerInterface
{
	GENERATED_BODY()

public:

	virtual void OnTimeChanged(float CurrentHour) = 0;
};
