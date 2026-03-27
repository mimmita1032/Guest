// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GuestNPCBase.h"
#include "Guest/AI/Controllers/GuestAIController.h"
#include "Guest/Utils/GLog.h"

AGuestNPCBase::AGuestNPCBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AGuestAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 기본값은 배회하는 일반 주민
	NPCType = ENPCType::Ambient;
}

void AGuestNPCBase::Interact(AActor* Interactor)
{
	// NPC 역할에 따라 상호작용 로직 분기
	if (NPCType == ENPCType::QuestGiver)
	{
		G_LOG(TEXT("퀘스트 NPC와 대화 시작!"));
	}
	else if (NPCType == ENPCType::Ambient)
	{
		G_LOG(TEXT("일반 주민 상호작용: 짧은 인사말 ㅇㅇ"));
	}
}