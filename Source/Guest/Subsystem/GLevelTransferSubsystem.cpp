// Copyright (c) 2026 Anything Left Behind?. All rights reserved.



#include "GLevelTransferSubsystem.h"
#include "Guest/Utils/GLog.h"

void UGLevelTransferSubsystem::SetTargetSpawnTag(FName InTag)
{
	TargetSpawnTag = InTag;
	UE_LOG(LogTemp, Log, TEXT("레벨 이동 서브시스템: 목적지 태그가 [%s](으)로 저장되었습니다."), *TargetSpawnTag.ToString());
}

void UGLevelTransferSubsystem::SetTransferTime(float InTime)
{
	StoredTime = InTime;
	UE_LOG(LogTemp, Log, TEXT("레벨 이동 서브시스템: 보존할 시간이 [%f]로 저장되었습니다."), StoredTime);
}

void UGLevelTransferSubsystem::ClearTransferData()
{
	TargetSpawnTag = NAME_None;
	StoredTime = -1.0f;
	UE_LOG(LogTemp, Log, TEXT("레벨 이동 서브시스템: 이동 데이터가 초기화되었습니다."));
}
