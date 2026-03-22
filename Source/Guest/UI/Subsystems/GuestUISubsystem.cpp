// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GuestUISubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Guest/UI/GameplayTags/GuestGameplayTags.h"
#include "Guest/Utils/GLog.h"

void UGuestUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	G_LOG(TEXT("Guest UI 서브시스템 가동 준비 완료"));
}

void UGuestUISubsystem::RegisterStack(FGameplayTag StackTag, UCommonActivatableWidgetStack* InStack)
{
	if (InStack)
	{
		Stacks.Add(StackTag, InStack);
		G_LOG(TEXT("UI 레이어 스택 등록 완료: %s"), *StackTag.ToString());
	}
}

FGameplayTag UGuestUISubsystem::ResolveStackTagForWidget(FGameplayTag WidgetTag)
{
	if (WidgetTag == GuestGameplayTags::TAG_Widget_PauseMenu)
	{
		return GuestGameplayTags::TAG_WidgetStack_GameMenu;
	}
	
	return GuestGameplayTags::TAG_WidgetStack_Frontend;
}

void UGuestUISubsystem::PushWidgetByTag(FGameplayTag WidgetTag)
{
	FGameplayTag TargetStackTag = ResolveStackTagForWidget(WidgetTag);

	if (UCommonActivatableWidgetStack* FoundStack = Stacks.FindRef(TargetStackTag))
	{
		// TODO: 실제 위젯 로드 로직
		G_LOG(TEXT("위젯 푸시 요청 성공! 태그: %s -> 대상 레이어: %s"), *WidgetTag.ToString(), *TargetStackTag.ToString());
	}
	else
	{
		G_ERR(TEXT("에러! 대상 레이어 스택을 찾을 수 없습니다: %s"), *TargetStackTag.ToString());
	}
}

