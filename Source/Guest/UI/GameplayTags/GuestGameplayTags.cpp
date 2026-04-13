// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/GameplayTags/GuestGameplayTags.h"

namespace GuestGameplayTags
{
    // ── Widget Stack Layers ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_Frontend, "Guest.WidgetStack.Frontend");
    UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_GameHUD,  "Guest.WidgetStack.GameHUD");
    UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_GameMenu, "Guest.WidgetStack.GameMenu");
    UE_DEFINE_GAMEPLAY_TAG(TAG_WidgetStack_Modal,    "Guest.WidgetStack.Modal");

    // ── Frontend ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_PressAnyKey,   "Guest.Widget.PressAnyKey");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_MainMenu,      "Guest.Widget.MainMenu");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_Options,       "Guest.Widget.Options");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_LoadingScreen, "Guest.Widget.LoadingScreen");

    // ── GameHUD ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_GameHUD,           "Guest.Widget.GameHUD");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_InteractionPrompt, "Guest.Widget.InteractionPrompt");

    // ── GameMenu ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_CameraUI,     "Guest.Widget.CameraUI");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_PauseMenu,    "Guest.Widget.PauseMenu");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_QuestJournal, "Guest.Widget.QuestJournal");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_Inventory,    "Guest.Widget.Inventory");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_TavernManage, "Guest.Widget.TavernManage");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_NPCDialogue,  "Guest.Widget.NPCDialogue");

    // ── Modal ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_Confirm,        "Guest.Widget.Confirm");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_Notification,   "Guest.Widget.Notification");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Widget_ItemDiscovered, "Guest.Widget.ItemDiscovered");
}
