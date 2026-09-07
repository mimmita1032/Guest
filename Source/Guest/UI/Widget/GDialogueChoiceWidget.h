// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Guest/Data/DataTable/GDialogueTypes.h"
#include "GDialogueChoiceWidget.generated.h"

class UCommonTextBlock;
class UCommonButtonBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChoiceSelected, FName /* NextNodeID */);

/**
 * 대화 선택지 버튼 위젯.
 * GuestDialogueWidgetBase의 Box_Choices에 동적으로 추가된다.
 * WBP_DialogueChoice를 Parent로 하는 Blueprint로 레이아웃 구성.
 *
 * 필수 BindWidget:
 *  - Btn_Choice        : CommonButtonBase 버튼
 *  - Text_ChoiceContent: 선택지 텍스트
 */
UCLASS(Abstract)
class GUEST_API UGDialogueChoiceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnChoiceSelected OnChoiceSelected;

	void SetupChoice(const FDialogueChoice& Choice);

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> Btn_Choice;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Text_ChoiceContent;

	/** 선택지 라벨 줄바꿈 기준 폭(px). 0이면 부모 슬롯이 준 폭에서 접는다. */
	UPROPERTY(EditAnywhere, Category = "Dialogue|Layout", meta = (ClampMin = "0.0"))
	float ChoiceWrapTextAt = 0.f;

	FName CachedNextNodeID;

	UFUNCTION()
	void OnClicked();
};
