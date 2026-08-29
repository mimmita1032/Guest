// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GuestGameplayTags.h"

namespace GuestGameplayTags
{
	//UI태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_Frontend, "Guest.WidgetStack.Frontend");
	UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_GameHUD, "Guest.WidgetStack.GameHUD");
	UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_GameMenu, "Guest.WidgetStack.GameMenu");
	UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_Modal, "Guest.WidgetStack.Modal");
	UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_BarDialogue, "Guest.WidgetStack.BarDialogue");
	UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_Narration, "Guest.WidgetStack.Narration");

	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_PressAnyKey, "Guest.Widget.PressAnyKey");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_PauseMenu, "Guest.Widget.PauseMenu");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_GameHUD, "Guest.Widget.GameHUD");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_MainMenu, "Guest.Widget.MainMenu");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_Options, "Guest.Widget.Options");
	
	

	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_Inventory, "Guest.Widget.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_Digicam, "Guest.Widget.Digicam");

	//세이브/ 로드 위젯 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_SaveBoard, "Guest.Widget.SaveBoard");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_LoadBoard, "Guest.Widget.LoadBoard");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_MainMenuSaveBoard, "Guest.Widget.MainMenuSaveBoard");
	
	// NPC 대화 위젯 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_NPCDialogue, "Guest.Widget.NPCDialogue");

	// 바 모드 대화 위젯 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_BarDialogue, "Guest.Widget.BarDialogue");

	// 나레이션 위젯 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_Narration, "Guest.Widget.Narration");

	//아이템 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Type_Weapon, "Item.Type.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Type_Consumable, "Item.Type.Consumable");
	
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Era_1995, "Item.Era.1995");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Era_2026, "Item.Era.2026");

	// ★ 테스트용 장비 부위 태그 2개 추가
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equip_Helmet, "Item.Equip.Helmet");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Equip_Chest, "Item.Equip.Chest");
	
	//테스트 인풋
	UE_DEFINE_GAMEPLAY_TAG(TAG_InputTag_Test, "Guest.InputTag.Test");
	
	//인풋
	UE_DEFINE_GAMEPLAY_TAG(TAG_InputTag_Interact, "Guest.InputTag.Interact");

	//Ability
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Interact, "Guest.Ability.Interact");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Flash,    "Guest.Ability.Flash");

	//인풋
	UE_DEFINE_GAMEPLAY_TAG(TAG_InputTag_Flash,   "Guest.InputTag.Flash");

	//State
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Dead,              "Guest.State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Blinded,           "Guest.State.Blinded");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Alert_Calm,        "Guest.State.Alert.Calm");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Alert_Suspicious,  "Guest.State.Alert.Suspicious");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Alert_Alerted,     "Guest.State.Alert.Alerted");
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_Alert_Chasing,     "Guest.State.Alert.Chasing");

	//Enemy 타입 식별 태그
	UE_DEFINE_GAMEPLAY_TAG(TAG_Enemy_Type_Reality,    "Guest.Enemy.Type.Reality");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Enemy_Type_AfterImage, "Guest.Enemy.Type.AfterImage");

	//GE SetByCaller 데이터
	UE_DEFINE_GAMEPLAY_TAG(TAG_Data_PurifyAmount, "Guest.Data.PurifyAmount");
	UE_DEFINE_GAMEPLAY_TAG(TAG_Data_DamageAmount, "Guest.Data.DamageAmount");

}
