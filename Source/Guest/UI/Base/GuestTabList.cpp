// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestTabList.h"
#include "GuestCommonTabButtonBase.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

void UGuestTabList::HandleTabCreation_Implementation(FName TabNameID, UCommonButtonBase* TabButton)
{
	// 부모 로직 실행 (CommonUI 내부 처리를 위해 필수)
	Super::HandleTabCreation_Implementation(TabNameID, TabButton);

	if (!HBox_Tabs || !TabButton) return;

	// 1. 노드에서 했던 Add Child to Horizontal Box를 코드로 처리
	UHorizontalBoxSlot* HBoxSlot = HBox_Tabs->AddChildToHorizontalBox(TabButton);
	if (HBoxSlot)
	{
		// 탭 버튼들이 가로 상자 안에서 일정한 비율로 채워지도록 설정
		FSlateChildSize SizeRule;
		SizeRule.SizeRule = ESlateSizeRule::Fill;
		HBoxSlot->SetSize(SizeRule);
	}

	// 2. 형변환 및 텍스트 설정 (Switch on Name 노드 대체)
	if (UGuestCommonTabButtonBase* GuestTab = Cast<UGuestCommonTabButtonBase>(TabButton))
	{
		if (TabNameID == FName("Tab_General"))
		{
			GuestTab->SetTabName(FText::FromString(TEXT("일반")));
		}
		else if (TabNameID == FName("Tab_Graphics"))
		{
			GuestTab->SetTabName(FText::FromString(TEXT("그래픽")));
		}
		else if (TabNameID == FName("Tab_Sound"))
		{
			GuestTab->SetTabName(FText::FromString(TEXT("소리")));
		}
		else if (TabNameID == FName("Tab_Controls"))
		{
			GuestTab->SetTabName(FText::FromString(TEXT("입력")));
		}
	}
}