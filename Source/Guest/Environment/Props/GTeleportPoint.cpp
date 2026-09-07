// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GTeleportPoint.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Guest/Utils/GLog.h"
#include "Kismet/GameplayStatics.h"

AGTeleportPoint::AGTeleportPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(Root);
	InteractionVolume->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionVolume->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AGTeleportPoint::Interact_Implementation(AActor* Interactor)
{
	if (DestinationTag.IsNone())
	{
		G_WARN(TEXT("GTeleportPoint: DestinationTag가 비어 있습니다. [%s]"), *GetName());
		return;
	}

	ACharacter* Player = Cast<ACharacter>(Interactor);
	if (!Player) return;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsWithTag(this, DestinationTag, Found);
	if (Found.Num() == 0)
	{
		G_WARN(TEXT("GTeleportPoint: [%s] 태그를 가진 도착 지점을 찾지 못했습니다."), *DestinationTag.ToString());
		return;
	}

	const AActor* Destination = Found[0];

	// TeleportPhysics로 옮겨야 캐릭터 캡슐이 도착 지점 지오메트리에 끼지 않는다.
	Player->SetActorLocation(Destination->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);

	// 액터만 돌리면 3인칭 카메라는 그대로다. 시선은 컨트롤러가 들고 있다.
	if (bFaceDestinationRotation)
	{
		if (AController* PC = Player->GetController())
		{
			PC->SetControlRotation(FRotator(0.f, Destination->GetActorRotation().Yaw, 0.f));
		}
	}

	G_LOG(TEXT("GTeleportPoint: [%s]로 이동"), *DestinationTag.ToString());
}

FText AGTeleportPoint::GetInteractText_Implementation() const
{
	return InteractText;
}
