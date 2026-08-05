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
 *   포위 중 타깃 방향 보정은 Behavior Tree의 BTS_OrientToTarget 서비스가 Actor Rotation을 직접 제어한다.
 *   평상시 배회/이동 중 회전은 CharacterMovement::bOrientRotationToMovement가 담당한다.
 * - NPC 전용 AGuestAIController와는 완전히 분리된 책임을 가진다.
 */
UCLASS()
class GUEST_API AGuestAfterimageAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGuestAfterimageAIController(const FObjectInitializer& ObjectInitializer);

	// IGenericTeamAgentInterface — Other(Pawn)의 Controller Team과 비교해 Friendly/Hostile/Neutral 판정.
	// Other가 Pawn이 아니거나 Controller가 Team을 구현하지 않으면 Neutral.
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	// Blackboard 키 이름 상수 (BT, EQS Context 등 외부에서도 참조 가능)
	static const FName BB_TargetActor;
	static const FName BB_StrafeLocation;
	static const FName BB_HasAttackToken;

	// Coordinator가 경계 알람 그룹 참가자로 판단했을 때 호출한다.
	// Blackboard TargetActor만 설정하며, MoveTo/EQS/공격을 직접 실행하지 않는다.
	// Behavior Tree가 값 변화를 감지해 Combat 브랜치로 전환하게 한다.
	void ReceiveSharedCombatTarget(AActor* TargetActor);

	// Coordinator가 그룹 전투 종료를 판단했을 때 호출한다.
	// 전달받은 TargetActor가 현재 Blackboard 값과 같을 때만 토큰 반환 및 Clear를 수행한다.
	void ClearSharedCombatTarget(AActor* TargetActor);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	// SightDataAsset 수치를 SightConfig에 적용하고 PerceptionComponent를 갱신
	void ApplySightDataAsset();

	// CrowdFollowingComponent를 안전하게 가져와 Crowd 관련 프로퍼티를 적용
	void ApplyCrowdSettings();

	// 감지 이벤트 핸들러 — Coordinator에 감지/상실을 보고한다 (SetFocus/ClearFocus 호출 안 함)
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// 보유 중인 공격권이 있으면 Coordinator에 반환하고 Blackboard의 HasAttackToken을 false로 정리한다.
	// 현재 TargetActor 감지 상실(그룹 해제 시)과 OnUnPossess 두 지점에서 호출된다.
	void ReleaseAttackTokenIfHeld();

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

	// 이 잔상이 플레이어를 직접 감지했을 때, 주변 잔상에게 같은 TargetActor를 공유해 Combat에 참여시킬 반경(cm)
	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat", meta = (ClampMin = "0.0"))
	float AlertRadius = 1500.0f;

	// Crowd Simulation 활성화 여부
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd")
	bool bEnableCrowdSimulation = true;

	// Crowd Avoidance(타 에이전트/장애물 회피) 활성화 여부
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd", meta = (editcondition = "bEnableCrowdSimulation"))
	bool bEnableCrowdAvoidance = true;

	// Crowd Avoidance 품질 (Low/Medium/Good/High) — 값이 높을수록 부드럽지만 비용이 커짐
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd", meta = (editcondition = "bEnableCrowdSimulation"))
	EGuestCrowdAvoidanceQuality CrowdAvoidanceQuality = EGuestCrowdAvoidanceQuality::High;

	// Crowd Separation(에이전트 간 간격 유지) 활성화 여부
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd", meta = (editcondition = "bEnableCrowdSimulation"))
	bool bEnableCrowdSeparation = true;

	// Separation 강도. 기본값 2.0은 UCrowdFollowingComponent 생성자의 엔진 기본값과 동일
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd", meta = (editcondition = "bEnableCrowdSimulation", ClampMin = "0.0"))
	float CrowdSeparationWeight = 2.0f;

	// true면 Crowd가 이동 속도 방향으로 캐릭터를 강제 회전시킴 — 회전은 CharacterMovement/BTS_OrientToTarget이 전담하므로 기본 false
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd", meta = (editcondition = "bEnableCrowdSimulation"))
	bool bCrowdRotateToVelocity = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd", meta = (editcondition = "bEnableCrowdSimulation"))
	float CollisionQueryRange = 600.f;
};