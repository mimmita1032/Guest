// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "GItemPlacementComponent.generated.h"

class UGItemDefinition;
class AGItemPlacementGhost;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlacementStateChanged, bool, bActive);

// 인벤토리 아이템을 자유배치(고스트 프리뷰 → 확정) 방식으로 월드에 내려놓는 기능.
// GuestCharacter에 부착. 배치 모드 중에만 Tick으로 마우스 커서 아래 지점을 트레이스해 고스트를 따라다니게 함.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUEST_API UGItemPlacementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGItemPlacementComponent();

	// 배치 모드 시작 — SourceHandle은 확정 시 인벤토리에서 제거할 아이템
	UFUNCTION(BlueprintCallable, Category = "Placement")
	void BeginPlacement(const UGItemDefinition* ItemDef, FInventoryItemHandle SourceHandle);

	// 현재 고스트 위치가 유효하면 배치 확정 (인벤토리에서 제거 + AGItemPickup 스폰)
	UFUNCTION(BlueprintCallable, Category = "Placement")
	void ConfirmPlacement();

	// 배치 취소 — 인벤토리 변화 없이 고스트만 제거
	UFUNCTION(BlueprintCallable, Category = "Placement")
	void CancelPlacement();

	UFUNCTION(BlueprintPure, Category = "Placement")
	bool IsPlacementActive() const { return bIsPlacementActive; }

	// GInventoryWidget이 구독해서 인벤토리 반투명화/드래그 잠금에 사용
	UPROPERTY(BlueprintAssignable, Category = "Placement")
	FOnPlacementStateChanged OnPlacementStateChanged;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 배치 가능 표면을 찾는 트레이스 최대 거리
	UPROPERTY(EditDefaultsOnly, Category = "Placement")
	float TraceDistance = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Placement")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	// 배치 중 보여줄 고스트 액터 클래스 — 에디터에서 반투명 머티리얼 지정한 BP로 세팅
	UPROPERTY(EditDefaultsOnly, Category = "Placement")
	TSubclassOf<AGItemPlacementGhost> GhostClass;

private:
	void UpdateGhostTransform();
	void EndPlacement();

	UPROPERTY()
	TObjectPtr<const UGItemDefinition> PendingItemDef;

	FInventoryItemHandle PendingSourceHandle;

	UPROPERTY()
	TObjectPtr<AGItemPlacementGhost> GhostActor;

	bool bIsPlacementActive = false;
	bool bIsValidPlacement = false;
};
