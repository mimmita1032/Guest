// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "GuestNPCBase.generated.h"

// NPC의 역할 구분용
UENUM(BlueprintType)
enum class ENPCType : uint8
{
	Ambient UMETA(DisplayName = "일반 주민 (배회용)"),
	QuestGiver UMETA(DisplayName = "퀘스트/대화 NPC")
};

UCLASS()
class GUEST_API AGuestNPCBase : public ACharacter, public IGInteractableInterface
{
	GENERATED_BODY()

public:
	AGuestNPCBase();

	// 상호작용 규약 구현: 플레이어가 E키를 눌렀을 때 실행될 함수
	virtual void Interact(AActor* Interactor) override;

public:
	// NPC 역할
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guest|NPC")
	ENPCType NPCType;

};