// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 * 
 */

namespace GuestGameplayTags
{
	//UI 태그
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WidgetStack_Frontend); //최하단 타이틀, 메인 메뉴, 설정
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WidgetStack_GameHUD); //2 시간대, 수거 목표, 상호작용 프롬프트
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WidgetStack_GameMenu); //3 수거 일지, 인벤토리, NPC 대화
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WidgetStack_Modal); //최상단 확인창, 아이템 발견 연출

	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_PressAnyKey);
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_PauseMenu);

	// 인벤토리 위젯 식별 태그
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_Inventory);

	//상호작용 프롬프트 위젯 태그
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_InteractionPrompt);

	//아이템 태그
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Type_Weapon);
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Type_Consumable);
	
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Era_1995);
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Era_2026);
	
	//테스트 인풋
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Test);
	
	//인풋
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Interact);

	//Ability
	GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Interact);
}

