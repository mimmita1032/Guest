// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "GuestPawnUIComponent.generated.h"

class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPawnStatChanged, float, Current, float, Max);

/**
 * UGuestPawnUIComponent
 *
 * Player Pawn에 부착되어 ASC Attribute 변경을 HUD Delegate로 중계하는 컴포넌트.
 * Widget은 ASC / AttributeSet을 직접 참조하지 않고 이 컴포넌트의 Delegate만 구독한다.
 *
 * 확장:
 *   Health / Battery 외에 Stamina, 상태이상, 상호작용 프롬프트 등
 *   Pawn 관련 UI 데이터가 늘어나면 이 컴포넌트에 Delegate를 추가한다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUEST_API UGuestPawnUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGuestPawnUIComponent();

	// PossessedBy / BeginPlay 직후 ASC가 준비된 시점에 호출
	void InitializeWithASC(UAbilitySystemComponent* InASC);

	// Widget이 구독 직후 현재 값을 즉시 받을 때 호출
	UFUNCTION(BlueprintCallable, Category = "UI|Stat")
	void RequestCurrentValues();

	// ── Delegates ──
	// Widget_GameHUD가 이 두 Delegate만 구독한다
	UPROPERTY(BlueprintAssignable, Category = "UI|Stat")
	FOnPawnStatChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "UI|Stat")
	FOnPawnStatChanged OnBatteryChanged;

private:
	void HandleHealthChanged(const FOnAttributeChangeData& Data);
	void HandleBatteryChanged(const FOnAttributeChangeData& Data);

	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	FDelegateHandle HealthChangedHandle;
	FDelegateHandle BatteryChangedHandle;
};
