// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "NativeGameplayTags.h"

/**
 * GuestGameplayTags.h
 *
 * UI 시스템에서 사용하는 모든 GameplayTag 선언.
 *
 *  Guest.WidgetStack.*  → Widget Stack 레이어 식별자
 *  Guest.Widget.*       → 개별 위젯 식별자
 */

namespace GuestGameplayTags
{
    // ── Widget Stack Layers ──
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WidgetStack_Frontend);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WidgetStack_GameHUD);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WidgetStack_GameMenu);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WidgetStack_Modal);

    // ── Frontend ──
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_PressAnyKey);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_MainMenu);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_Options);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_LoadingScreen);

    // ── GameHUD ──
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_GameHUD);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_InteractionPrompt);

    // ── GameMenu ──
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_CameraUI);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_PauseMenu);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_QuestJournal);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_Inventory);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_TavernManage);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_NPCDialogue);

    // ── Modal ──
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_Confirm);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_Notification);
   GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Widget_ItemDiscovered);
}
