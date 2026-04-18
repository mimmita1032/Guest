// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GInventoryComponent.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Items/Instance/GItemInstance.h"
UGInventoryComponent::UGInventoryComponent()
{
	// 인벤토리 데이터 저장소는 매 프레임 업데이트될 필요가 없으므로 Tick을 꺼서 성능을 최적화합니다.
	PrimaryComponentTick.bCanEverTick = false; 
}

void UGInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// 1. 총 슬롯 개수 계산
	const int32 TotalSlots = Columns * Rows;

	// 2. 배열 초기화 (nullptr로 빈 공간 할당)
	InventorySlots.Init(nullptr, TotalSlots);

	// 3. 초기화 결과 한글 로그 출력
	UE_LOG(LogGSystem, Log, TEXT("인벤토리 컴포넌트 초기화 완료. 크기: %d x %d (총 %d 슬롯)"), Columns, Rows, TotalSlots);
}

