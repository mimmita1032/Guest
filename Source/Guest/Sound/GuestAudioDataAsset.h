// GuestAudioDataAsset.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AkAudioEvent.h"
#include "AkRtpc.h"
#include "AkSwitchValue.h"
#include "AkStateValue.h"
#include "GuestAudioDataAsset.generated.h"

/**
 * FGuestAudioAction
 * 특정 상황(태그)에서 실행될 Wwise 오디오 데이터들의 묶음입니다.
 */
USTRUCT(BlueprintType)
struct FGuestAudioAction
{
	GENERATED_BODY()

	// 재생할 Wwise 오디오 이벤트 (예: 사운드 재생, 중지 등)
	UPROPERTY(EditAnywhere, Category = "Wwise")
	TObjectPtr<UAkAudioEvent> Event = nullptr;

	// 실시간 제어 파라미터(RTPC) 설정: RTPC 에셋과 그에 따른 수치값
	UPROPERTY(EditAnywhere, Category = "Wwise")
	TMap<TObjectPtr<UAkRtpc>, float> RTPCs;

	// 스위치 설정: 특정 사운드 변형을 선택하기 위한 Wwise Switch 값들
	UPROPERTY(EditAnywhere, Category = "Wwise")
	TArray<TObjectPtr<UAkSwitchValue>> Switches;

	// 상태 설정: 게임의 전역적인 오디오 상태를 변경하기 위한 Wwise State 값들
	UPROPERTY(EditAnywhere, Category = "Wwise")
	TArray<TObjectPtr<UAkStateValue>> States;
};


/**
 * UGuestAudioDataAsset
 * GameplayTag를 키로 사용하여 오디오 액션을 찾을 수 있게 해주는 데이터 에셋입니다.
 * 하드코딩을 방지하고 에디터에서 사운드 구성을 관리하기 용이하게 합니다.
 */
UCLASS(BlueprintType)
class GUEST_API UGuestAudioDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/* 사운드 식별 태그와 오디오 액션을 매핑하는 테이블*/
	UPROPERTY(EditAnywhere, Category = "Audio Config", meta = (Categories = "Sound"))
	TMap<FGameplayTag, FGuestAudioAction> AudioMap;

	/** * 특정 태그에 해당하는 오디오 액션 정보를 찾아 반환합니다.
	     * @param Tag 찾고자 하는 사운드 태그
	     * @return 해당 태그의 오디오 액션 포인터 (없을 경우 nullptr)
	     */
	const FGuestAudioAction* FindAction(const FGameplayTag& Tag) const { return AudioMap.Find(Tag); }
};
