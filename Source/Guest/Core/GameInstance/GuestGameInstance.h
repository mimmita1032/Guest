#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Guest/Save/GuestSaveGame.h"
#include "GuestGameInstance.generated.h"
UCLASS()



class GUEST_API UGuestGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	virtual void Shutdown() override;
	
	/** 슬롯에서 읽고, 같은 맵이면 즉시 적용 / 다른 맵이면 OpenLevel 후 적용 */
	void RequestLoadFromSlot(const FString& SlotName, int32 UserIndex = 0);

	/** 세이브/로드가 아닌 레벨 전환(디지캠 시공간 이동 등) 직전에 호출 — 위치는 새 레벨 기준 그대로 두고 인벤토리/GAS만 다음 맵으로 이어붙임 */
	void CarryPlayerStateAcrossTravel();

protected:
	void OnPostLoadMapWithWorld(UWorld* LoadedWorld);
	
	FDelegateHandle PostLoadMapDelegateHandle;
	
	bool bPendingApplyPlayerWorld = false;
	FGuestPlayerWorldState PendingPlayerWorld;
	
private:
	// GAS 어트리뷰트(체력/배터리)를 세이브 데이터에서 복원
	static void RestoreGASAttributes(APawn* Pawn, const UGuestSaveGame* SaveObject);

	// 인벤토리 복원 + 사진 스냅샷 재생성을 한 묶음으로 처리.
	// 사진이 인벤토리 아이템이라 둘은 항상 함께 가야 하므로 호출부를 분리하지 않는다.
	void RestoreInventory(APawn* Pawn, const UGuestSaveGame* SaveObject);
	// 맵 전환 시 GAS 복원에 쓸 세이브 오브젝트 임시 보관
	UPROPERTY()
	TObjectPtr<UGuestSaveGame> PendingSaveObject;
};