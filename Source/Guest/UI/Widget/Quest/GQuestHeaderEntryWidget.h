// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GQuestHeaderEntryWidget.generated.h"

class UTextBlock;

// 퀘스트 목록에서 퀘스트 제목 + 설명 한 줄을 표시하는 헤더 위젯
UCLASS()
class GUEST_API UGQuestHeaderEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(const FString& InQuestName, const FString& InDescription);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_QuestName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Description;

	/** 제목·설명 줄바꿈 기준 폭(px). 0이면 부모 슬롯이 준 폭에서 접는다. */
	UPROPERTY(EditAnywhere, Category = "Quest|Layout", meta = (ClampMin = "0.0"))
	float HeaderWrapTextAt = 0.f;

	virtual void NativeOnInitialized() override;
};
