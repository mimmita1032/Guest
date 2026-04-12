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
};