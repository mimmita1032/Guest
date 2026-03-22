// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "GuestUISubsystem.generated.h"

class UCommonActivatableWidgetStack;

UCLASS()
class GUEST_API UGuestUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// PrimaryLayout에서 4개의 Stack을 전달받아 기억해두는 함수
	UFUNCTION(BlueprintCallable, Category = "Guest|UI")
	void RegisterStack(FGameplayTag StackTag, UCommonActivatableWidgetStack* InStack);

	// 태그(예: TAG_Widget_PauseMenu)를 던지면 알맞은 층에 위젯을 띄워줌
	UFUNCTION(BlueprintCallable, Category = "Guest|UI")
	void PushWidgetByTag(FGameplayTag WidgetTag);

private:
	// 위젯 태그를 보고 라우팅해주기
	FGameplayTag ResolveStackTagForWidget(FGameplayTag WidgetTag);

	// 등록된 Stack들을 태그와 함께 보관
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetStack>> Stacks;
};