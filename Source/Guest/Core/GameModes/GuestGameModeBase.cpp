// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Core/GameModes/GuestGameModeBase.h"
#include "Guest/Characters/Player/GuestCharacter.h"
#include "Guest/Core/Controllers/GuestPlayerController.h"
#include "Guest/Core/States/GuestGameState.h"
#include "Guest/Core/States/GuestPlayerState.h"
#include "Guest/Subsystem/GSpacetimeSubsystem.h"
#include "Guest/Subsystem/GLevelTransferSubsystem.h"
#include "GameFramework/PlayerStart.h"
#include "Guest/Utils/GLog.h"
#include "Kismet/GameplayStatics.h"

AGuestGameModeBase::AGuestGameModeBase()
{
	
	DefaultPawnClass = AGuestCharacter::StaticClass();
	PlayerControllerClass = AGuestPlayerController::StaticClass();
	PlayerStateClass = AGuestPlayerState::StaticClass();
	GameStateClass = AGuestGameState::StaticClass();

#pragma region TimeTick
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
#pragma endregion // TimeTick
}


#pragma region TimeTick
void AGuestGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGSpacetimeSubsystem* SpacetimeSS = GI->GetSubsystem<UGSpacetimeSubsystem>())
		{
			SpacetimeSS->UpdateWorldTime(DeltaTime);
		}
	}
}
#pragma endregion

AActor* AGuestGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	// 1. 서브시슽ㅔㅁ에서 목적지 태그 가져오기
	if (UGLevelTransferSubsystem* TransferSubsystem = GetGameInstance()->GetSubsystem<UGLevelTransferSubsystem>())
	{
		FName TargetTag = TransferSubsystem->GetTargetSpawnTag();

		// 태그가 비어있지 않다면
		if (TargetTag != NAME_None)
		{
			// 월드에 배치된 모든 PlayerStart 액터를 모음
			TArray<AActor*> FoundStarts;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundStarts);

			// 하나씩 확인하며 우리가 찾는 태그와 일치하는지 대조
			for (AActor* Actor : FoundStarts)
			{
				APlayerStart* StartPos = Cast<APlayerStart>(Actor);
				if (StartPos && StartPos->PlayerStartTag == TargetTag)
				{
					// 찾았다면 데이터를 지우고 해당 위치 반환
					TransferSubsystem->ClearTransferData();
					
					G_LOG(TEXT("스폰 매니저: [%s] 태그 위치에서 플레이어를 스폰합니다."), *TargetTag.ToString());
					return StartPos;
				}
			}
			
			G_WARN(TEXT("스폰 매니저: [%s] 태그를 가진 PlayerStart를 찾을 수 없습니다! 기본 위치로 스폰합니다."), *TargetTag.ToString());
		}
	}

	// 6. 저장된 태그가 없거나 예외 발생 시, 엔진 원래 방식대로 스폰
	return Super::ChoosePlayerStart_Implementation(Player);
}