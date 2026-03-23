// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/AI/Controllers/GuestAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h" // 필수 포함
#include "Guest/Utils/GLog.h"

AGuestAIController::AGuestAIController()
{

}

void AGuestAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BTAsset)
	{

		if (RunBehaviorTree(BTAsset))
		{
			G_LOG(TEXT("AI 행동 트리 작동 시작: %s"), *InPawn->GetName());
		}
		else
		{
			G_ERR(TEXT("행동 트리 실행 실패!"));
		}
	}
	else
	{
		G_WARN(TEXT("BTAsset이 설정되지 않았습니다."));
	}
}