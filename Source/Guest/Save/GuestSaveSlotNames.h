#pragma once
#include "CoreMinimal.h"
namespace GuestSaveSlots
{
	/** 로컬 싱글 등: 보통 0. SaveGameToSlot / LoadGameFromSlot / DoesSaveGameExist 의 UserIndex. */
	int32 DefaultUserIndex();
	/**
	 * UI 슬롯 인덱스(0 .. NumSlots-1)에 대응하는 세이브 슬롯 이름.
	 * PopulateSlots / 저장 / 로드 / 존재 여부 검사에서 동일하게 사용.
	 */
	FString MakeSlotName(int32 SlotIndex);
}