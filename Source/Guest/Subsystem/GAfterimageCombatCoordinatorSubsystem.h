// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GAfterimageCombatCoordinatorSubsystem.generated.h"

class AGuestAfterimageAIController;

/**
 * 잔상(AfterImage) 포위 전투의 최소 공격권(Attack Token)과 경계 알람(Alert) 그룹을 함께 관리하는 Coordinator.
 * - 공격권: TargetActor 한 명당 공격 행동을 수행할 수 있는 잔상을 최대 한 마리로 제한한다.
 * - 알람: 한 잔상이 플레이어를 직접 감지하면 AlertRadius 내 등록된 잔상들도 같은 TargetActor로 Combat에 참여시킨다.
 *   직접 감지 중인 잔상(DirectObservers)이 모두 사라졌을 때만 그룹 전체를 해제한다.
 * Tick을 사용하지 않으며, 등록/보고/조회 시점에만 무효화된 항목을 정리한다.
 */
UCLASS()
class GUEST_API UGAfterimageCombatCoordinatorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// --- 공격 토큰 ---

	// TargetActor에 대한 공격권을 RequestingAfterimage에게 부여를 시도한다.
	// 이미 다른 잔상이 보유 중이면 실패, 보유자가 없거나 RequestingAfterimage 본인이 이미 보유 중이면 성공.
	bool TryAcquireAttackToken(AActor* TargetActor, AActor* RequestingAfterimage);

	// RequestingAfterimage가 TargetActor에 대한 공격권을 보유 중이면 반환한다. 보유자가 아니면 안전하게 무시한다.
	void ReleaseAttackToken(AActor* TargetActor, AActor* RequestingAfterimage);

	// RequestingAfterimage가 TargetActor에 대한 공격권을 현재 보유 중인지 조회한다.
	bool HasAttackToken(AActor* TargetActor, AActor* RequestingAfterimage) const;

	// --- 잔상 Controller 등록 ---

	// OnPossess() 성공 후 호출한다. 중복 등록은 안전하게 무시된다.
	void RegisterAfterimageController(AGuestAfterimageAIController* Controller);

	// OnUnPossess()에서 호출한다.
	void UnregisterAfterimageController(AGuestAfterimageAIController* Controller);

	// --- 경계 알람 ---

	// Observer가 TargetActor를 감지/상실했음을 보고한다.
	// 감지 성공 시 Observer의 AlertRadius 내에 등록된 잔상들을 같은 TargetActor의 알람 그룹에 편입시키고
	// 각 참가자에게 ReceiveSharedCombatTarget()을 전달한다.
	// 감지 상실 시 Observer를 그룹의 DirectObservers에서 제거하며, DirectObservers가 모두 사라졌을 때만
	// 그룹 전체에 ClearSharedCombatTarget()을 전달하고 공격권과 그룹 데이터를 정리한다.
	void ReportTargetSensed(AGuestAfterimageAIController* Observer, AActor* TargetActor, bool bSuccessfullySensed, float AlertRadius);

private:
	// TargetActor별 경계 알람 그룹 상태.
	// DirectObservers   : 플레이어를 현재 Sight로 직접 감지 중인 잔상.
	// AlertedControllers: 알람을 받아 같은 TargetActor와 전투 중인 잔상(DirectObservers 포함).
	struct FAfterimageAlertGroup
	{
		TWeakObjectPtr<AActor> TargetActor;
		TSet<TWeakObjectPtr<AGuestAfterimageAIController>> DirectObservers;
		TSet<TWeakObjectPtr<AGuestAfterimageAIController>> AlertedControllers;
	};

	// TargetActor당 현재 공격권을 보유한 잔상. 강한 참조를 피하기 위해 TWeakObjectPtr를 사용한다.
	mutable TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<AActor>> CurrentAttackersByTarget;

	// TargetActor당 알람 그룹.
	TMap<TWeakObjectPtr<AActor>, FAfterimageAlertGroup> AlertGroupsByTarget;

	// 알람 반경 검색 대상이 되는 활성 잔상 Controller 등록소 — 매 보고마다 월드 전체를 검색하지 않기 위함.
	TSet<TWeakObjectPtr<AGuestAfterimageAIController>> RegisteredControllers;
};
