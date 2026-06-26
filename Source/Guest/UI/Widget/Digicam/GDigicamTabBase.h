// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GDigicamTabBase.generated.h"

// 디지캠 내 모든 탭 페이지의 베이스 클래스
// 각 탭(아이템, 퀘스트, 사진 등)은 이 클래스를 BP에서 상속받아 제작
UCLASS(Abstract, BlueprintType)
class GUEST_API UGDigicamTabBase : public UUserWidget
{
	GENERATED_BODY()

public:
	// 탭 활성화 시 데이터 갱신용 (C++ 서브클래스는 _Implementation override)
	UFUNCTION(BlueprintNativeEvent, Category = "Digicam|Tab")
	void OnTabActivated();
	virtual void OnTabActivated_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Digicam|Tab")
	void OnTabDeactivated();
	virtual void OnTabDeactivated_Implementation() {}
};
