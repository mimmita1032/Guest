// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Guest/Components/Interaction/GInteractionComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Components/DGDigicamComponent.h"

// Sets default values
AGuestCharacter::AGuestCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	InteractionComponent = CreateDefaultSubobject<UGInteractionComponent>(TEXT("InteractionComponent"));

	DigicamComponent = CreateDefaultSubobject<UDGDigicamComponent>(TEXT("DigicamComponent")); // 추가

}

void AGuestCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AGuestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (UInputAction* RawMove = Cast<UInputAction>(IA_Move.Get()))
		{
			EIC->BindAction(RawMove, ETriggerEvent::Triggered, this, &AGuestCharacter::MoveAction);
		}

		if (UInputAction* RawLook = Cast<UInputAction>(IA_Look.Get()))
		{
			EIC->BindAction(RawLook, ETriggerEvent::Triggered, this, &AGuestCharacter::LookAction);
		}

		if (UInputAction* RawJump = Cast<UInputAction>(IA_Jump.Get()))
		{
			EIC->BindAction(RawJump, ETriggerEvent::Started, this, &AGuestCharacter::JumpAction);
			EIC->BindAction(RawJump, ETriggerEvent::Completed, this, &ACharacter::StopJumping); 
		}

		if (InteractAction)
		{
			EIC->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AGuestCharacter::OnInteract);
		} // 여기서 하나만 닫아야 합니다.

		// 디카 관련 바인딩이 EIC 중괄호 안으로 들어와야 함
		if (IA_DigicamControl)
		{
			EIC->BindAction(IA_DigicamControl, ETriggerEvent::Triggered, this, &AGuestCharacter::DigicamControlAction);
		}

		if (IA_DigicamShutter)
		{
			EIC->BindAction(IA_DigicamShutter, ETriggerEvent::Started, this, &AGuestCharacter::DigicamShutterAction);
		}

		if (IA_DigicamToggle)
		{
			EIC->BindAction(IA_DigicamToggle, ETriggerEvent::Started, this, &AGuestCharacter::DigicamToggleAction);
		}
	}
}

void AGuestCharacter::MoveAction(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGuestCharacter::LookAction(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGuestCharacter::JumpAction(const FInputActionValue& Value)
{
	Jump();
	UE_LOG(LogTemp, Log, TEXT("점프 실행"));
}

void AGuestCharacter::OnInteract(const FInputActionValue& Value)
{
	if (InteractionComponent)
	{
		G_LOG(TEXT("상호작용 키 입력됨"));
		InteractionComponent->DoInteract();
	}
}

#pragma region Digicam
void AGuestCharacter::DigicamControlAction(const FInputActionValue& Value)
{
	FVector2D ControlVector = Value.Get<FVector2D>();

	if (DigicamComponent)
	{
		// X값은 좌우(Horizontal), Y값은 상하(Vertical)로 전달
		DigicamComponent->HandleHorizontalInput(ControlVector.X);
		DigicamComponent->HandleVerticalInput(ControlVector.Y);
	}
}

void AGuestCharacter::DigicamShutterAction(const FInputActionValue& Value)
{
	if (DigicamComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("디카 셔터 눌림"));
		DigicamComponent->HandleShutter();
	}
}

void AGuestCharacter::DigicamToggleAction(const FInputActionValue& Value)
{
	if (DigicamComponent)
	{
		// 간단한 토글 로직: 비활성이면 켜고, 아니면 끔
		// 나중에 상태에 따라 더 정교하게 다듬을 예정
		static bool bIsActive = false;
		bIsActive = !bIsActive;

		if (bIsActive) DigicamComponent->ActivateDigicam();
		else DigicamComponent->DeactivateDigicam();
	}
}
#pragma endregion