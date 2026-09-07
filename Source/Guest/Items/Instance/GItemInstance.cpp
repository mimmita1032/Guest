// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GItemInstance.h"
#include "Guest/Utils/GLog.h"

FInventoryItemHandle::FInventoryItemHandle()
	:HandleId{GetInvalidId()}
{
}

FInventoryItemHandle FInventoryItemHandle::InvalidHandle()
{
	static FInventoryItemHandle InvalidHandle = FInventoryItemHandle();
	return InvalidHandle;
}

FInventoryItemHandle FInventoryItemHandle::CreateHandle() 
{
	return FInventoryItemHandle(GenerateNextId());
}

bool FInventoryItemHandle::IsValid() const
{
	return HandleId != GetInvalidId();
}

FInventoryItemHandle::FInventoryItemHandle(uint32 Id)
	:HandleId(Id)
{ 
}

uint32 FInventoryItemHandle::GenerateNextId()
{
	static uint32 StaticId = 1;
	return StaticId++;
}

uint32 FInventoryItemHandle::GetInvalidId()
{
	return 0;
}

void UGItemInstance::SetInstanceData(const FInstancedStruct& InInstanceData)
{
	// 빈 구조체 주입은 "개체 데이터 없음"으로 되돌리는 정상 동작
	if (!InInstanceData.IsValid())
	{
		InstanceData.Reset();
		return;
	}

	// 세이브에서 임의 구조체가 넘어올 수 있으므로 계보를 확인한다 — 아니면 통째로 무시
	const UScriptStruct* Struct = InInstanceData.GetScriptStruct();
	if (!Struct || !Struct->IsChildOf(FGItemInstanceData::StaticStruct()))
	{
		G_WARN(TEXT("SetInstanceData 거부: [%s]는 FGItemInstanceData 파생이 아닙니다."),
			Struct ? *Struct->GetName() : TEXT("None"));
		return;
	}

	InstanceData = InInstanceData;
}
