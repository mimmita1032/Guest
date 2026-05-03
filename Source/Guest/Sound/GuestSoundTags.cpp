// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Sound/GuestSoundTags.h"

namespace GuestSoundTags
{
    // ── Event: UI ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_UI_ButtonClick,        "Guest.Sound.Event.UI.ButtonClick");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_UI_ButtonHover,        "Guest.Sound.Event.UI.ButtonHover");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_UI_ScreenOpen,         "Guest.Sound.Event.UI.ScreenOpen");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_UI_ScreenClose,        "Guest.Sound.Event.UI.ScreenClose");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_UI_ItemDiscovered,     "Guest.Sound.Event.UI.ItemDiscovered");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_UI_CollectionComplete, "Guest.Sound.Event.UI.CollectionComplete");

    // ── Event: BGM ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_BGM_Here_Play,  "Guest.Sound.Event.BGM.Here.Play");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_BGM_There_Play, "Guest.Sound.Event.BGM.There.Play");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_BGM_Stop,       "Guest.Sound.Event.BGM.Stop");

    // ── Event: Ambience ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_Ambience_Tavern,  "Guest.Sound.Event.Ambience.Tavern");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_Ambience_Outdoor, "Guest.Sound.Event.Ambience.Outdoor");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_Ambience_Stop,    "Guest.Sound.Event.Ambience.Stop");

    // ── Event: SFX ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_SFX_Footstep,    "Guest.Sound.Event.SFX.Footstep");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_SFX_ItemPickup,  "Guest.Sound.Event.SFX.ItemPickup");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_SFX_TravelThere, "Guest.Sound.Event.SFX.TravelThere");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_SFX_ReturnHere,  "Guest.Sound.Event.SFX.ReturnHere");

    // ── Event: Voice ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Event_Voice_DialogueStart, "Guest.Sound.Event.Voice.DialogueStart");

    // ── RTPC ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_RTPC_Volume_BGM,          "Guest.Sound.RTPC.Volume.BGM");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_RTPC_Volume_SFX,          "Guest.Sound.RTPC.Volume.SFX");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_RTPC_Volume_UI,           "Guest.Sound.RTPC.Volume.UI");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_RTPC_Volume_Ambience,     "Guest.Sound.RTPC.Volume.Ambience");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_RTPC_Player_Speed,        "Guest.Sound.RTPC.Player.Speed");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_RTPC_TimePeriod_Progress, "Guest.Sound.RTPC.TimePeriod.Progress");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_RTPC_Weather_Intensity,   "Guest.Sound.RTPC.Weather.Intensity");

    // ── State: Location ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_Location_Here,  "Guest.Sound.State.Location.Here");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_Location_There, "Guest.Sound.State.Location.There");

    // ── State: TimePeriod ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_Time_Dawn,      "Guest.Sound.State.Time.Dawn");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_Time_Morning,   "Guest.Sound.State.Time.Morning");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_Time_Afternoon, "Guest.Sound.State.Time.Afternoon");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_Time_Evening,   "Guest.Sound.State.Time.Evening");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_Time_Night,     "Guest.Sound.State.Time.Night");

    // ── State: Weather ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_Weather_Clear,  "Guest.Sound.State.Weather.Clear");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_Weather_Cloudy, "Guest.Sound.State.Weather.Cloudy");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_Weather_Rain,   "Guest.Sound.State.Weather.Rain");

    // ── State: GameMode ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_GameMode_Gameplay, "Guest.Sound.State.GameMode.Gameplay");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_GameMode_MainMenu, "Guest.Sound.State.GameMode.MainMenu");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_State_GameMode_Paused,   "Guest.Sound.State.GameMode.Paused");

    // ── Switch: Surface ──
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Switch_Surface_Stone,  "Guest.Sound.Switch.Surface.Stone");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Switch_Surface_Dirt,   "Guest.Sound.Switch.Surface.Dirt");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Switch_Surface_Wood,   "Guest.Sound.Switch.Surface.Wood");
    UE_DEFINE_GAMEPLAY_TAG(TAG_Sound_Switch_Surface_Metal,  "Guest.Sound.Switch.Surface.Metal");
}
