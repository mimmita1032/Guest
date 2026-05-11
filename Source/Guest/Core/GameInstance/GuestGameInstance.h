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
	
protected:
	void OnPostLoadMapWithWorld(UWorld* LoadedWorld);
	
	FDelegateHandle PostLoadMapDelegateHandle;
	
	bool bPendingApplyPlayerWorld = false;
	FGuestPlayerWorldState PendingPlayerWorld;
	
private:
	// GAS 어트리뷰트(체력/배터리)를 세이브 데이터에서 복원
	static void RestoreGASAttributes(APawn* Pawn, const UGuestSaveGame* SaveObject);
	// 맵 전환 시 GAS 복원에 쓸 세이브 오브젝트 임시 보관
	UPROPERTY()
	TObjectPtr<UGuestSaveGame> PendingSaveObject;
};