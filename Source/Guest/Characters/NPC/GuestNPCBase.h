// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "GuestNPCBase.generated.h"

class UGNPCScheduleDataAsset;

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

	virtual void BeginPlay() override;

	// 상호작용 규약 구현: 플레이어가 E키를 눌렀을 때 실행될 함수
	virtual void Interact_Implementation(AActor* Interactor) override;

public:
	// NPC 역할
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guest|NPC")
	ENPCType NPCType;

	// 시간대별 이동 스케줄 데이터 에셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guest|NPC|Schedule")
	TObjectPtr<UGNPCScheduleDataAsset> ScheduleDataAsset;

private:
	// GSpacetimeSubsystem::OnTimeChanged 델리게이트 핸들러
	UFUNCTION()
	void HandleTimeChanged(float CurrentHour);

	// 현재 시각에 맞는 스케줄 목적지를 BB에 반영
	void ApplySchedule(float CurrentHour);

	// 직전에 적용된 스케줄의 목적지 태그 (변경 감지용)
	FName LastActiveScheduleTag = NAME_None;
};