// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "NativeGameplayTags.h"

/**
 * GuestSoundTags.h
 *
 * Wwise 사운드 시스템에서 사용하는 모든 GameplayTag 선언.
 * GuestGameplayTags.h 와 동일한 NativeGameplayTag 방식 사용.
 *
 *  Guest.Sound.Event.*   → Wwise Event 트리거 (PostEvent 에 사용)
 *  Guest.Sound.RTPC.*    → 실시간 파라미터 제어 (SetRTPCValue 에 사용)
 *  Guest.Sound.State.*   → 전역 상태 전환 (SetState 에 사용)
 *  Guest.Sound.Switch.*  → 로컬 사운드 분기 (SetSwitch 에 사용)
 */
namespace GuestSoundTags
{
    // ── Event: UI ──
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_UI_ButtonClick);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_UI_ButtonHover);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_UI_ScreenOpen);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_UI_ScreenClose);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_UI_ItemDiscovered);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_UI_CollectionComplete);

    // ── Event: BGM ──
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_BGM_Here_Play);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_BGM_There_Play);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_BGM_Stop);

    // ── Event: Ambience ──
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_Ambience_Tavern);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_Ambience_Outdoor);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_Ambience_Stop);

    // ── Event: SFX ──
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_SFX_Footstep);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_SFX_ItemPickup);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_SFX_TravelThere);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_SFX_ReturnHere);

    // ── Event: Voice ──
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Event_Voice_DialogueStart);

    // ── RTPC ──
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_RTPC_Volume_BGM);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_RTPC_Volume_SFX);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_RTPC_Volume_UI);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_RTPC_Volume_Ambience);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_RTPC_Player_Speed);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_RTPC_TimePeriod_Progress);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_RTPC_Weather_Intensity);

    // ── State: Location ──
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_Location_Here);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_Location_There);

    // ── State: TimePeriod ──
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_Time_Dawn);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_Time_Morning);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_Time_Afternoon);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_Time_Evening);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_Time_Night);

    // ── State: Weather ──
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_Weather_Clear);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_Weather_Cloudy);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_Weather_Rain);

    // ── State: GameMode ──
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_GameMode_Gameplay);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_GameMode_MainMenu);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_State_GameMode_Paused);

    // ── Switch: Surface ──
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Switch_Surface_Stone);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Switch_Surface_Dirt);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Switch_Surface_Wood);
    GUEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Sound_Switch_Surface_Metal);
}
