// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"

// 프로젝트 전역 Team ID 상수.
// Player / Enemy(현실+잔상 공통)에만 유효한 Team ID를 부여한다.
// NPC는 이번 작업에서 Team ID를 부여받지 않는다.
namespace GuestTeamIds
{
	inline constexpr uint8 Player = 1;
	inline constexpr uint8 Enemy = 2;
}
