// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "GItemInstance.h"

FInventoryItemHandle::FInventoryItemHandle()
	:HandleId{GetInvalidId()}
{
}

FInventoryItemHandle FInventoryItemHandle::InvalidHandle()
{
	static FInventoryItemHandle InvalidHandle = FInventoryItemHandle();
	return InvalidHandle;
}

FInventoryItemHandle FInventoryItemHandle::CreateHandele()
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
