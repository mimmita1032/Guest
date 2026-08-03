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
};
