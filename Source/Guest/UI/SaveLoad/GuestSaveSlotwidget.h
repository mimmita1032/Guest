#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GuestSaveSlotWidget.generated.h"
UCLASS()
class GUEST_API UGuestSaveSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetSlotIndex(int32 InIndex) { SlotIndex = InIndex; }
	int32 GetSlotIndex() const { return SlotIndex; }
	/** 세이브 없으면 false — 빈 칸/대시 표시 */
	void SetCreateAtDisplay(const FText& InText);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_CreateAt;
private:
	int32 SlotIndex = INDEX_NONE;
};