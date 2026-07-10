// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GDigicamTabBase.h"
#include "GDigicamQuestWidget.generated.h"

class UVerticalBox;
class UWidget;
class UGQuestSubsystem;
class UGObjectiveEntryWidget;
class UGQuestHeaderEntryWidget;

// 디지캠 퀘스트 탭 — 진행 중인 퀘스트 이름과 현재 단계 목표를 표시
UCLASS(Abstract)
class GUEST_API UGDigicamQuestWidget : public UGDigicamTabBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	// 탭 진입 시 목록 즉시 갱신
	virtual void OnTabActivated_Implementation() override;

protected:
	// 퀘스트 이름 + 목표 목록이 쌓이는 세로 목록 (WBP_DigiTab_Quest에서 BindWidget 이름 맞출 것)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> Box_QuestList;

	// 목표 한 줄을 표시하는 위젯 클래스 (기존 WBP_ObjectiveEntry 재사용)
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	TSubclassOf<UGObjectiveEntryWidget> ObjectiveEntryClass;

	// 퀘스트 제목 + 설명을 표시하는 헤더 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	TSubclassOf<UGQuestHeaderEntryWidget> QuestHeaderClass;

	// 진행 중인 퀘스트가 하나도 없을 때 표시할 안내 위젯 (선택적)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UWidget> WGT_EmptyHint;

private:
	UFUNCTION()
	void RefreshQuestList();

	UGQuestSubsystem* GetQuestSubsystem() const;
};
