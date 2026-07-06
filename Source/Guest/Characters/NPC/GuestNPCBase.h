// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "GuestNPCBase.generated.h"

class UGNPCScheduleDataAsset;
class UGDialogueDataAsset;

// NPC의 역할 구분용
UENUM(BlueprintType)
enum class ENPCType : uint8
{
	Ambient    UMETA(DisplayName = "일반 주민 (배회용)"),
	QuestGiver UMETA(DisplayName = "퀘스트/대화 NPC")
};

UCLASS()
class GUEST_API AGuestNPCBase : public ACharacter, public IGInteractableInterface
{
	GENERATED_BODY()

public:
	AGuestNPCBase();

	virtual void BeginPlay() override;

	virtual void Interact_Implementation(AActor* Interactor) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guest|NPC")
	ENPCType NPCType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guest|NPC|Schedule")
	TObjectPtr<UGNPCScheduleDataAsset> ScheduleDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guest|NPC|Dialogue")
	TObjectPtr<UGDialogueDataAsset> DialogueAsset;

private:
	UFUNCTION()
	void HandleTimeChanged(float CurrentHour);

	void ApplySchedule(float CurrentHour);

	FName LastActiveScheduleTag = NAME_None;
};
