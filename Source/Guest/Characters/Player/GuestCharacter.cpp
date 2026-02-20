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
			EIC->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AGuestCharacter::OnInteract);		}
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