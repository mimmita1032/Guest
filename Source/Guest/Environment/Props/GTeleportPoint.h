// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "GTeleportPoint.generated.h"

class UBoxComponent;

// 같은 레벨 안에서 플레이어를 다른 지점으로 옮기는 상호작용 지점.
// AGDoorLevelTransition은 레벨을 갈아끼우므로 한 레벨 안의 이동에는 쓸 수 없다.
//
// 스카이스파이어 전망대처럼 "올라가는 과정"을 만들 비용이 큰 구간을 건너뛰는 데 쓴다.
// 도착 지점은 DestinationTag를 가진 액터를 레벨에서 찾아 정한다.
UCLASS()
class GUEST_API AGTeleportPoint : public AActor, public IGInteractableInterface
{
	GENERATED_BODY()

public:
	AGTeleportPoint();

	// 도착 지점 액터의 Tags에 넣어둔 값. 빈 값이면 아무 일도 하지 않는다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport")
	FName DestinationTag;

	// 상호작용 안내 문구. 비우면 표시하지 않는다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport")
	FText InteractText;

	// 도착 지점 액터의 방향으로 시선을 맞출지 여부.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport")
	bool bFaceDestinationRotation = true;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractText_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;
};
