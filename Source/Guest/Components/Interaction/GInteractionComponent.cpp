// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInteractionComponent.h"
#include "Guest/Components/Interaction/GInteractionComponent.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "Guest/Utils/GLog.h"
#include "GameFramework/Character.h"

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
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	FVector Start;
	FRotator Rotation;
	Owner->GetActorEyesViewPoint(Start, Rotation);

	FVector End = Start + (Rotation.Vector() * InteractionRange);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner); // 자신은 제외

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		AActor* HitActor = HitResult.GetActor();
		
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UGInteractableInterface::StaticClass()))
		{
			return HitActor;
		}
	}

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