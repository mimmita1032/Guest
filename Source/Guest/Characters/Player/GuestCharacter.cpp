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
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = TargetZoomLength;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = true;

	InteractionComponent = CreateDefaultSubobject<UGInteractionComponent>(TEXT("InteractionComponent"));

	DigicamComponent = CreateDefaultSubobject<UDGDigicamComponent>(TEXT("DigicamComponent"));

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

void AGuestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 셀카봉 길이와 목표 길이가 0.1 이상 차이가 날 때만 연산 수행 (최적화 때매)
	if (!FMath::IsNearlyEqual(SpringArmComp->TargetArmLength, TargetZoomLength, 0.1f))
	{
		// FInterpTo: 현재 값에서 타겟 값으로 DeltaTime에 맞춰 부드럽게 보간(이동)
		SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, TargetZoomLength, DeltaTime, ZoomSpeed);
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
		} 

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

		if (IA_Zoom)
		{
			EIC->BindAction(IA_Zoom, ETriggerEvent::Triggered, this, &AGuestCharacter::ZoomAction);
		}

		if (IA_FreeLook)
		{
			EIC->BindAction(IA_FreeLook, ETriggerEvent::Started, this, &AGuestCharacter::FreeLookStart);
			EIC->BindAction(IA_FreeLook, ETriggerEvent::Completed, this, &AGuestCharacter::FreeLookEnd);
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
#pragma region Zoom, FreeLook
void AGuestCharacter::ZoomAction(const FInputActionValue& Value)
{
	float ZoomInput = Value.Get<float>();

	TargetZoomLength -= (ZoomInput * ZoomStep);
	
	TargetZoomLength = FMath::Clamp(TargetZoomLength, MinZoomLength, MaxZoomLength);
}

void AGuestCharacter::FreeLookStart(const FInputActionValue& Value)
{
	bIsFreeLooking = true;
	bUseControllerRotationYaw = false;
	G_LOG(TEXT("자유 시점 시작: 캐릭터는 가만히 있고 카메라만 돕니다.")); 
}

void AGuestCharacter::FreeLookEnd(const FInputActionValue& Value)
{
	bIsFreeLooking = false;
	bUseControllerRotationYaw = true;
	G_LOG(TEXT("자유 시점 종료: 카메라가 다시 제자리로 돌아갑니다."));
}
#pragma endregion