// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Subsystem/GAfterimageCombatCoordinatorSubsystem.h"
#include "Guest/AI/Controllers/GuestAfterimageAIController.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

bool UGAfterimageCombatCoordinatorSubsystem::TryAcquireAttackToken(AActor* TargetActor, AActor* RequestingAfterimage)
{
	if (!IsValid(TargetActor) || !IsValid(RequestingAfterimage))
	{
		return false;
	}

	const TWeakObjectPtr<AActor> TargetKey(TargetActor);
	if (TWeakObjectPtr<AActor>* ExistingAttacker = CurrentAttackersByTarget.Find(TargetKey))
	{
		if (ExistingAttacker->IsValid())
		{
			return ExistingAttacker->Get() == RequestingAfterimage;
		}

		// 보유자가 무효화됨 — 정리 후 아래에서 새로 등록
		CurrentAttackersByTarget.Remove(TargetKey);
	}

	CurrentAttackersByTarget.Add(TargetKey, TWeakObjectPtr<AActor>(RequestingAfterimage));
	return true;
}

void UGAfterimageCombatCoordinatorSubsystem::ReleaseAttackToken(AActor* TargetActor, AActor* RequestingAfterimage)
{
	if (!IsValid(TargetActor) || !IsValid(RequestingAfterimage))
	{
		return;
	}

	const TWeakObjectPtr<AActor> TargetKey(TargetActor);
	TWeakObjectPtr<AActor>* ExistingAttacker = CurrentAttackersByTarget.Find(TargetKey);
	if (!ExistingAttacker)
	{
		return;
	}

	if (!ExistingAttacker->IsValid())
	{
		CurrentAttackersByTarget.Remove(TargetKey);
		return;
	}

	if (ExistingAttacker->Get() != RequestingAfterimage)
	{
		// 보유자가 아닌 Actor의 반환 요청은 안전하게 무시
		return;
	}

	CurrentAttackersByTarget.Remove(TargetKey);
}

bool UGAfterimageCombatCoordinatorSubsystem::HasAttackToken(AActor* TargetActor, AActor* RequestingAfterimage) const
{
	if (!IsValid(TargetActor) || !IsValid(RequestingAfterimage))
	{
		return false;
	}

	const TWeakObjectPtr<AActor> TargetKey(TargetActor);
	TWeakObjectPtr<AActor>* ExistingAttacker = CurrentAttackersByTarget.Find(TargetKey);
	if (!ExistingAttacker)
	{
		return false;
	}

	if (!ExistingAttacker->IsValid())
	{
		CurrentAttackersByTarget.Remove(TargetKey);
		return false;
	}

	return ExistingAttacker->Get() == RequestingAfterimage;
}

void UGAfterimageCombatCoordinatorSubsystem::RegisterAfterimageController(AGuestAfterimageAIController* Controller)
{
	if (!Controller)
	{
		return;
	}

	RegisteredControllers.Add(TWeakObjectPtr<AGuestAfterimageAIController>(Controller));
}

void UGAfterimageCombatCoordinatorSubsystem::UnregisterAfterimageController(AGuestAfterimageAIController* Controller)
{
	if (!Controller)
	{
		return;
	}

	RegisteredControllers.Remove(TWeakObjectPtr<AGuestAfterimageAIController>(Controller));
}

void UGAfterimageCombatCoordinatorSubsystem::ReportTargetSensed(AGuestAfterimageAIController* Observer, AActor* TargetActor, bool bSuccessfullySensed, float AlertRadius)
{
	if (!IsValid(Observer) || !IsValid(TargetActor))
	{
		return;
	}

	const TWeakObjectPtr<AActor> TargetKey(TargetActor);
	const TWeakObjectPtr<AGuestAfterimageAIController> ObserverKey(Observer);

	if (bSuccessfullySensed)
	{
		APawn* ObserverPawn = Observer->GetPawn();
		if (!IsValid(ObserverPawn))
		{
			return;
		}

		FAfterimageAlertGroup& Group = AlertGroupsByTarget.FindOrAdd(TargetKey);
		Group.TargetActor = TargetKey;
		Group.DirectObservers.Add(ObserverKey);
		Group.AlertedControllers.Add(ObserverKey);

		// Observer Pawn을 중심으로 AlertRadius 내 등록된 잔상을 알람 참가자로 편입 (자기 자신 포함 가능)
		const FVector Origin = ObserverPawn->GetActorLocation();
		const float RadiusSquared = FMath::Square(AlertRadius);

		for (auto It = RegisteredControllers.CreateIterator(); It; ++It)
		{
			AGuestAfterimageAIController* Candidate = It->Get();
			if (!Candidate)
			{
				// 무효화된 등록 항목 정리
				It.RemoveCurrent();
				continue;
			}

			APawn* CandidatePawn = Candidate->GetPawn();
			if (!IsValid(CandidatePawn))
			{
				continue;
			}

			if (FVector::DistSquared(Origin, CandidatePawn->GetActorLocation()) <= RadiusSquared)
			{
				Group.AlertedControllers.Add(TWeakObjectPtr<AGuestAfterimageAIController>(Candidate));
			}
		}

		// 참여 Controller 전체에게 같은 TargetActor 전달 — 이미 같은 타깃을 가진 Controller에 다시 전달해도 안전
		for (auto It = Group.AlertedControllers.CreateIterator(); It; ++It)
		{
			AGuestAfterimageAIController* Alerted = It->Get();
			if (!Alerted)
			{
				It.RemoveCurrent();
				continue;
			}

			Alerted->ReceiveSharedCombatTarget(TargetActor);
		}

		return;
	}

	// 감지 상실
	FAfterimageAlertGroup* Group = AlertGroupsByTarget.Find(TargetKey);
	if (!Group)
	{
		return;
	}

	Group->DirectObservers.Remove(ObserverKey);

	// 무효화된 Direct Observer 정리
	for (auto It = Group->DirectObservers.CreateIterator(); It; ++It)
	{
		if (!It->IsValid())
		{
			It.RemoveCurrent();
		}
	}

	if (Group->DirectObservers.Num() > 0)
	{
		// 다른 Direct Observer가 남아 있으면 그룹 전투를 유지한다
		return;
	}

	// Direct Observer가 모두 사라짐 — 그룹 전체 해제
	for (auto It = Group->AlertedControllers.CreateIterator(); It; ++It)
	{
		if (AGuestAfterimageAIController* Alerted = It->Get())
		{
			Alerted->ClearSharedCombatTarget(TargetActor);
		}
	}

	// 공격 토큰 Map에도 해당 Target의 항목이 남지 않도록 방어적으로 정리
	CurrentAttackersByTarget.Remove(TargetKey);
	AlertGroupsByTarget.Remove(TargetKey);
}
