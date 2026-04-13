#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GuestSaveSlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGuestSaveSlotClicked, int32, SlotIndex);

UCLASS()
class GUEST_API UGuestSaveSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetSlotIndex(int32 InIndex) { SlotIndex = InIndex; }
	
	int32 GetSlotIndex() const { return SlotIndex; }
	
	/** 세이브 없으면 false — 빈 칸/대시 표시 */
	void SetCreateAtDisplay(const FText& InText);
	void setMainQuestDisplay();
	/** 저장/로드 보드가 구독: 클릭 시 SlotIndex와 함께 브로드캐스트 */
	UPROPERTY(BlueprintAssignable, Category = "Save|Slot")
	FOnGuestSaveSlotClicked OnSlotClicked;
	
protected:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_CreateAt;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_CurrentMainQuest;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Slot;
	
	
	
private:
	UFUNCTION()
	void HandleSlotButtonClicked();
	
	int32 SlotIndex = INDEX_NONE;
};