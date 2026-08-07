// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Guest/UI/Base/GuestActivatableBase.h"
#include "Guest/Data/DataTable/GDialogueTypes.h"
#include "GuestDialogueWidgetBase.generated.h"

class UCommonTextBlock;
class UCommonButtonBase;
class UVerticalBox;
class UGDialogueDataAsset;
class UGDialogueChoiceWidget;

/**
 * 대화 위젯 공통 베이스.
 * GBarDialogueWidget (바 모드)과 GuestNPCDialogueWidget (스카이림 스타일) 공통 로직.
 *
 * DataTable(FDialogueNode)을 읽어 노드 순서대로 진행하며,
 * 선택지 없으면 Btn_Next, 있으면 Box_Choices에 UGDialogueChoiceWidget을 동적 생성.
 *
 * 필수 BindWidget (WBP에서 구성):
 *  - Text_SpeakerName   : 화자 이름
 *  - Text_DialogueLine  : 대사 본문
 *  - Btn_Next           : 다음 버튼 (선택지 없을 때 표시)
 *  - Box_Choices        : 선택지 컨테이너 VerticalBox (선택지 있을 때 표시)
 */
UCLASS(Abstract)
class GUEST_API UGuestDialogueWidgetBase : public UGuestActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	/** 서브클래스 NativeOnActivated에서 호출. 세션 초기화 후 첫 노드 표시. */
	void StartDialogueSession(UGDialogueDataAsset* InAsset);

	/** 서브클래스 NativeOnDeactivated에서 호출. 세션 상태 초기화. */
	void ResetDialogueSession();

	UPROPERTY()
	TObjectPtr<UGDialogueDataAsset> CurrentDialogueAsset;

	FName CurrentNodeID;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Text_SpeakerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Text_DialogueLine;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> Btn_Next;

	/** Btn_Next 내부 텍스트. PlayerResponseText를 여기에 표시한다. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Text_PlayerResponse;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> Box_Choices;

	/** 선택지 버튼 위젯 클래스. WBP 에디터에서 WBP_DialogueChoice 지정. */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UGDialogueChoiceWidget> ChoiceWidgetClass;

	void ShowCurrentNode();
	void PopulateChoices(const TArray<FDialogueChoice>& Choices);
	void AdvanceTo(FName NextNodeID);

	/**
	 * 선택지 표시 조건 판정.
	 *  - 비어 있으면 항상 표시
	 *  - "Q_Smith_001"        — 그 퀘스트가 진행 중일 때
	 *  - "Q_Smith_001.Step02" — 진행 중이고 현재 단계가 Step02일 때
	 */
	bool IsChoiceConditionMet(FName ConditionID) const;

	UFUNCTION()
	void OnNextClicked();

	void OnChoiceSelected(FName NextNodeID);
};
