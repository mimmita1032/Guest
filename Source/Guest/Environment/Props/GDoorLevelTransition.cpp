// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Environment/Props/GDoorLevelTransition.h"
#include "Guest/Subsystem/GLevelTransferSubsystem.h"
#include "Guest/Utils/GLog.h"
#include "Kismet/GameplayStatics.h"

void AGDoorLevelTransition::Interact(AActor* Interactor)
{
	// 부모의 상호작용 로직(문 열리는 애니메이션, 소리 등 << 아직 안넣음)을 먼저 실행
	Super::Interact(Interactor);

	// 태그없으면 이동x
	if (TargetLevelName == NAME_None || TargetSpawnTag == NAME_None)
	{
		G_WARN(TEXT("레벨 이동 문: 목적지 레벨 이름이나 태그가 설정되지 않았습니다!"));
		return;
	}

	// 서브시스템 불러와 목적지 태그를 주입
	if (UGLevelTransferSubsystem* TransferSubsystem = GetGameInstance()->GetSubsystem<UGLevelTransferSubsystem>())
	{
		TransferSubsystem->SetTargetSpawnTag(TargetSpawnTag);
		G_LOG(TEXT("레벨 이동 문: 이동 준비 완료. 목적지 태그 [%s] 저장됨."), *TargetSpawnTag.ToString());
	}

	// 이동 함수를 호출하여 맵을 전환
	G_LOG(TEXT("레벨 이동 문: [%s] 레벨로 이동을 시작합니다."), *TargetLevelName.ToString());
	UGameplayStatics::OpenLevel(this, TargetLevelName);
}