// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GuestGameplayTags.h"

namespace GuestGameplayTags
{
	//UI태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_Frontend, "Guest.WidgetStack.Frontend");
	UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_GameHUD, "Guest.WidgetStack.GameHUD");
	UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_GameMenu, "Guest.WidgetStack.GameMenu");
	UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_Modal, "Guest.WidgetStack.Modal");

	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_PressAnyKey, "Guest.Widget.PressAnyKey");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_PauseMenu, "Guest.Widget.PauseMenu");

	//인벤토리 위젯 식별 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_Inventory, "Guest.Widget.Inventory");

	//상호작용 프롬프트 위젯 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_InteractionPrompt, "Guest.Widget.InteractionPrompt");

	//아이템 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Type_Weapon, "Item.Type.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Type_Consumable, "Item.Type.Consumable");
	
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Era_1995, "Item.Era.1995");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Era_2026, "Item.Era.2026");
	
	//테스트 인풋
	UE_DEFINE_GAMEPLAY_TAG(TAG_InputTag_Test, "Guest.InputTag.Test");
	
	//인풋
	UE_DEFINE_GAMEPLAY_TAG(TAG_InputTag_Interact, "Guest.InputTag.Interact");
	
	//Ability
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interact, "Guest.Ability.Interact");
	
}
