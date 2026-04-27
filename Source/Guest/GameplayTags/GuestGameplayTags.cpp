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
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_GameHUD, "Guest.Widget.GameHUD");
	

	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_Inventory, "Guest.Widget.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_Digicam, "Guest.Widget.Digicam");

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
