// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Guest/Data/DataAssets/GAISightDataAsset.h"
#include "GuestAfterimageAIController.generated.h"

// ECrowdAvoidanceQuality::Type(엔진 namespace enum)은 UHT가 리플렉션할 수 없어
// UPROPERTY로 직접 노출이 불가능하다. 동일한 4단계를 그대로 미러링한 UENUM으로 노출하고,
// 사용 시점(ApplyCrowdSettings)에 static_cast로 변환한다.
UENUM(BlueprintType)
enum class EGuestCrowdAvoidanceQuality : uint8
{
	Low,
	Medium,
	Good,
	High,
};

/**
 * AGuestAfterimageAIController
 *
 * 잔상(AfterImage) 타입 적 전용 AIController.
 * - PathFollowingComponent를 UCrowdFollowingComponent로 교체해 잔상끼리의 이동 충돌만 회피한다.
 * - TargetActor Blackboard 키만 설정/해제한다. Focus나 회전은 이 클래스가 다루지 않으며,
 *   회전은 Behavior Tree의 BTS_RotateToTarget 서비스가 Actor Rotation을 직접 제어한다.
 * - NPC 전용 AGuestAIController와는 완전히 분리된 책임을 가진다.
 */
UCLASS()
class GUEST_API AGuestAfterimageAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGuestAfterimageAIController(const FObjectInitializer& ObjectInitializer);

	// Blackboard 키 이름 상수 (BT, EQS Context 등 외부에서도 참조 가능)
	static const FName BB_TargetActor;
	static const FName BB_StrafeLocation;

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	// SightDataAsset 수치를 SightConfig에 적용하고 PerceptionComponent를 갱신
	void ApplySightDataAsset();

	// CrowdFollowingComponent를 안전하게 가져와 Crowd 관련 프로퍼티를 적용
	void ApplyCrowdSettings();

	// 감지 이벤트 핸들러 — TargetActor Blackboard 값만 설정/해제한다 (SetFocus/ClearFocus 호출 안 함)
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// 잔상 Crowd Agent끼리만 서로 회피하도록 구분하는 그룹 비트.
	// 플레이어/현실 적은 Crowd Agent가 아니므로 이 그룹의 영향을 받지 않는다.
	static constexpr int32 AfterimageCrowdGroup = 1 << 0;

protected:
	// 이 컨트롤러가 실행할 잔상 전투용 행동 트리
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	// 에디터에서 할당하는 시각 감지 수치 데이터 에셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<UGAISightDataAsset> SightDataAsset;

	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	// Crowd Simulation 활성화 여부
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd")
	bool bEnableCrowdSimulation = true;

	// Crowd Avoidance(타 에이전트/장애물 회피) 활성화 여부
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd")
	bool bEnableCrowdAvoidance = true;

	// Crowd Avoidance 품질 (Low/Medium/Good/High) — 값이 높을수록 부드럽지만 비용이 커짐
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd")
	EGuestCrowdAvoidanceQuality CrowdAvoidanceQuality = EGuestCrowdAvoidanceQuality::Medium;

	// Crowd Separation(에이전트 간 간격 유지) 활성화 여부
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd")
	bool bEnableCrowdSeparation = true;

	// Separation 강도. 기본값 2.0은 UCrowdFollowingComponent 생성자의 엔진 기본값과 동일
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd", meta = (ClampMin = "0.0"))
	float CrowdSeparationWeight = 2.0f;

	// true면 Crowd가 이동 속도 방향으로 캐릭터를 강제 회전시킴 — 회전은 BTS_RotateToTarget이 전담하므로 기본 false
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd")
	bool bCrowdRotateToVelocity = false;
};