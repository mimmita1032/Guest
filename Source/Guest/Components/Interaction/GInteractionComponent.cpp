// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInteractionComponent.h"
#include "Guest/Components/Interaction/GInteractionComponent.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "Guest/Utils/GLog.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"

UGInteractionComponent::UGInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FocusedActor = FindInteractable();
}

AActor* UGInteractionComponent::FindInteractable() const
{
	// 소유자 확인 및 시선 기준 시작점 설정
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;
	if (!GetWorld()) return nullptr;

	FVector Start;
	FRotator Rotation;
	Owner->GetActorEyesViewPoint(Start, Rotation);
	FVector End = Start + (Rotation.Vector() * InteractionRange);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner); // 본인 제외

	// 라인 트레이스 실행 (Visibility 채널 사용)
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	// 디버그 라인 그리기 (히트: 초록, 실패: 빨강)
	FColor DebugColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), Start, End, DebugColor, false, 0.1f, 0, 1.0f);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			// 인터페이스 구현 여부 확인
			if (HitActor->GetClass()->ImplementsInterface(UGInteractableInterface::StaticClass()))
			{
				return HitActor;
			}
		}
	}

	// 아무것도 맞지 않았거나 상호작용 객체가 아닐 경우
	return nullptr;
}

void UGInteractionComponent::DoInteract()
{
	if (FocusedActor)
	{
		if (IGInteractableInterface* Interactable = Cast<IGInteractableInterface>(FocusedActor))
		{
			G_LOG(TEXT("상호작용 실행: %s"), *FocusedActor->GetName());
			Interactable->Interact(GetOwner());
		}
	}
	else
	{
		G_WARN(TEXT("상호작용 대상이 없습니다."));
	}
}