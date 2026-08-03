// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestEnemyCharacter.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Guest/GAS/GuestAbilitySystemComponent.h"
#include "Guest/GAS/GuestAttributeSet.h"
#include "Guest/Data/DataAssets/GCharacterGASData.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"

AGuestEnemyCharacter::AGuestEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GuestASC = CreateDefaultSubobject<UGuestAbilitySystemComponent>(TEXT("GuestASC"));
	GuestAttributeSet = CreateDefaultSubobject<UGuestAttributeSet>(TEXT("GuestAttributeSet"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AGuestEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!GuestASC) return;

	GuestASC->InitAbilityActorInfo(this, this);

	if (UGCharacterGASData* LoadedData = EnemyGasData.LoadSynchronous())
	{
		LoadedData->GiveToASC(GuestASC);
	}

	GuestASC->RegisterGameplayTagEvent(
		GuestGameplayTags::TAG_State_Dead,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AGuestEnemyCharacter::OnDeadTagChanged);

	GuestASC->RegisterGameplayTagEvent(
		GuestGameplayTags::TAG_State_Blinded,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AGuestEnemyCharacter::OnBlindedTagChanged);

	if (EnemyIdentityTag.IsValid())
	{
		GuestASC->AddLooseGameplayTag(EnemyIdentityTag);
	}

	// 잔상은 평상시 배회/일반 Move To에서는 bOrientRotationToMovement로 이동 방향을 바라보고,
	// 포위(Strafe) 브랜치에서는 BTS_OrientToTarget 서비스가 매 Tick SetActorRotation()으로 타깃 방향을 추가 보정한다.
	// 두 방식 모두 항상 활성 상태를 유지하며 BT Service는 이 설정을 토글하지 않는다.
	// NPC 및 현실(Reality) 적의 회전 설정은 변경하지 않는다
	if (IsAfterimageEnemy())
	{
		bUseControllerRotationYaw = false;

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->bOrientRotationToMovement     = true;
			MoveComp->bUseControllerDesiredRotation = false;
			MoveComp->bUseRVOAvoidance              = false; // CrowdFollowingComponent와 동시 사용 방지
		}
	}
}

UAbilitySystemComponent* AGuestEnemyCharacter::GetAbilitySystemComponent() const
{
	return GuestASC;
}

bool AGuestEnemyCharacter::IsAfterimageEnemy() const
{
	return EnemyIdentityTag == GuestGameplayTags::TAG_Enemy_Type_AfterImage;
}

void AGuestEnemyCharacter::OnDeadTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	// NewOrRemoved 이벤트: Count 0→1 시 한 번만 발화
	if (NewCount > 0)
	{
		HandleDeath();
	}
}

void AGuestEnemyCharacter::HandleDeath()
{
	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->StopMovement();
		AIC->UnPossess();
	}

	// 사망 몽타주 / 이펙트는 Blueprint에서 처리
	// BlueprintNativeEvent로 필요 시 하위 클래스에서 Override 가능
}

void AGuestEnemyCharacter::OnBlindedTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	// Blinded가 해제(0)될 때만 Reality Enemy에 Calm 상태 부여
	if (NewCount > 0 || EnemyIdentityTag != GuestGameplayTags::TAG_Enemy_Type_Reality) return;

	if (!GuestASC->HasMatchingGameplayTag(GuestGameplayTags::TAG_State_Alert_Calm))
	{
		GuestASC->AddLooseGameplayTag(GuestGameplayTags::TAG_State_Alert_Calm);
	}
}
