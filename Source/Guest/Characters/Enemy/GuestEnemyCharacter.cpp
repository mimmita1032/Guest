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

	// 잔상은 CharacterMovement/Controller가 아닌 BTS_RotateToTarget 서비스가 Actor Rotation을 직접 제어하므로
	// 두 시스템이 동시에 회전을 갱신하지 않도록 관련 자동 회전 로직을 모두 꺼둔다.
	// NPC 및 현실(Reality) 적의 회전 설정은 변경하지 않는다
	if (IsAfterimageEnemy())
	{
		bUseControllerRotationYaw = false;

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->bOrientRotationToMovement     = false;
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
