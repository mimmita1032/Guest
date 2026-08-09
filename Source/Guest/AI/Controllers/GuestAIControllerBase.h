// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GuestAIControllerBase.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UGAISightDataAsset;

/**
 * AGuestAIControllerBase
 *
 * NPC(AGuestAIController) / GuardDog(AGuardDogAIController) / 잔상(AGuestAfterimageAIController)이
 * 공통으로 필요로 하는 Perception 초기화만 제공한다.
 * OnPossess 전체 흐름, HandleTargetPerceptionUpdated, Crowd/Team/Coordinator/Alert/AttackToken 등
 * 각 Controller 전용 로직은 이 클래스로 옮기지 않고 파생 클래스에 그대로 둔다.
 */
UCLASS(Abstract)
class GUEST_API AGuestAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	AGuestAIControllerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 3개 Controller가 동일 의미("TargetActor")로 사용하는 Blackboard 키
	static const FName BB_TargetActor;

	// NPC/GuardDog만 사용하는 키(시야 상실 시 마지막 감지 위치). 잔상은 사용하지 않는다.
	static const FName BB_LastKnownLocation;

protected:
	// SightConfig/AIPerceptionComp "객체 생성"만 담당한다.
	// DetectionByAffiliation, SightRadius 등 세부값은 파생 클래스 생성자가 채운다.
	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	// 파생 클래스 생성자가 SightConfig 세부값(DetectionByAffiliation 등)을 다 채운 "다음"에 호출해야 한다.
	// 순서를 바꾸면 ConfigureSense가 낡은 SightConfig 값을 스냅샷해 AIPerceptionComp에 반영되지 않는다.
	void FinalizeSightPerceptionSetup();

	// SightDataAsset 수치를 SightConfig에 반영하는 공통 로직.
	// SightDataAsset 프로퍼티 자체는 각 파생 클래스에 그대로 둔다(카테고리/BP 노출 차이 보존).
	void ApplySightDataAssetToConfig(const UGAISightDataAsset* InSightDataAsset);
};
