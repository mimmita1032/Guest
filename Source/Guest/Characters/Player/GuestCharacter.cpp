// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GuestCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Guest/Components/Interaction/GInteractionComponent.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Components/GDigicamComponent.h"
// 데이터 에셋 헤더 추가
#include "Guest/Data/DataAssets/GCharacterDataAsset.h"
//gas
#include "Guest/GAS/GuestAbilitySystemComponent.h"
#include "Guest/GAS/GuestAttributeSet.h"
//gas DATA
#include "Guest/DATA/DataAssets/GCharacterGASData.h"
//인풋 구성
#include "Guest/Data/Input/GInputConfigData.h"
#include "Guest/Components/Input/GuestInputComponent.h"

// Sets default values
AGuestCharacter::AGuestCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 15.0f));
    SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->bInheritPitch = true;
    SpringArmComp->bInheritYaw = true;
    SpringArmComp->bInheritRoll = true;
    
    SpringArmComp->bDoCollisionTest = true; 
    SpringArmComp->ProbeSize = 12.0f; 
    SpringArmComp->ProbeChannel = ECC_Camera;
    SpringArmComp->TargetOffset = FVector::ZeroVector;
    SpringArmComp->SocketOffset = FVector(0.0f, 75.0f, 0.0f);
    SpringArmComp->bEnableCameraRotationLag = true;
    SpringArmComp->CameraRotationLagSpeed = 15.0f;
    SpringArmComp->bEnableCameraLag = true;
    SpringArmComp->CameraLagSpeed = 12.0f;
    
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;
    
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

    InteractionComponent = CreateDefaultSubobject<UGInteractionComponent>(TEXT("InteractionComponent"));
    DigicamComponent = CreateDefaultSubobject<UGDigicamComponent>(TEXT("DigicamComponent"));
    
    // 연산용 초기값
    TargetZoomLength = 400.0f;
    ZoomSpeed = 10.0f;
   
   GuestAbilitySystemComponent = CreateDefaultSubobject<UGuestAbilitySystemComponent>(TEXT("GuestAbilitySystemComponent"));
   GuestAttributeSet = CreateDefaultSubobject<UGuestAttributeSet>(TEXT("GuestAttributeSet"));
}

void AGuestCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (SpringArmComp)
    {
       SpringArmComp->SetRelativeLocation(FVector::ZeroVector);
       SpringArmComp->SocketOffset = FVector::ZeroVector;
       SpringArmComp->TargetOffset = FVector(0.0f, 0.0f, 60.0f);
    }

    if (CameraComp)
    {
       CameraComp->SetRelativeLocation(FVector::ZeroVector);
    }
    
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
       if (PC->PlayerCameraManager)
       {
          PC->PlayerCameraManager->ViewPitchMin = -50.0f; 
          PC->PlayerCameraManager->ViewPitchMax = 60.0f;  
       }
       if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
       {
          Subsystem->ClearAllMappings();
          Subsystem->AddMappingContext(DefaultMappingContext, 0);
       }
    }

    // 데이터 에셋에서 수치 로드
   if (CharacterData)
   {
      GetCharacterMovement()->MaxWalkSpeed = CharacterData->WalkSpeed;
      GetCharacterMovement()->MaxWalkSpeedCrouched = CharacterData->CrouchSpeed;
      // 점프력 세팅 추가
      GetCharacterMovement()->JumpZVelocity = CharacterData->JumpZVelocity;

      // 줌 연산용 초기값 세팅 (초기 타겟 거리를 범위 안으로 클램핑)
      TargetZoomLength = FMath::Clamp(CharacterData->TargetZoomLength, CharacterData->MinZoomLength, CharacterData->MaxZoomLength);
      ZoomSpeed = CharacterData->ZoomSpeed;
       
      UE_LOG(LogTemp, Log, TEXT("캐릭터 데이터 에셋 로드 성공. 점프력: %f"), CharacterData->JumpZVelocity);
   }

    SpringArmComp->TargetArmLength = TargetZoomLength;
}

void AGuestCharacter::PossessedBy(AController* NewController)
{
   Super::PossessedBy(NewController);
   
   if (GuestAbilitySystemComponent)
   {
      GuestAbilitySystemComponent->InitAbilityActorInfo(this,this);
   }
   
   ensureMsgf(!CharacterGasData.IsNull(), TEXT("캐릭터 GAS 데이터 할당 안됨"));
   
   if (UGCharacterGASData* LoadedData = CharacterGasData.LoadSynchronous())
   {
      LoadedData->GiveToASC(GuestAbilitySystemComponent);
   }
}

void AGuestCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!FMath::IsNearlyEqual(SpringArmComp->TargetArmLength, TargetZoomLength, 0.1f))
    {
       SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, TargetZoomLength, DeltaTime, ZoomSpeed);
    }

    float FpsAlpha = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 450.0f), FVector2D(0.0f, 1.0f), SpringArmComp->TargetArmLength);

    SpringArmComp->SocketOffset.Y = FMath::Lerp(0.0f, 75.0f, FpsAlpha);
    SpringArmComp->SocketOffset.Z = FMath::Lerp(0.0f, 140.0f, FpsAlpha);
    
    if (SpringArmComp->TargetArmLength < 50.0f)
    {
       SpringArmComp->bDoCollisionTest = false;
    }
    else
    {
       SpringArmComp->bDoCollisionTest = true;
    }
    
    float TargetFOV = FMath::Lerp(100.0f, 90.0f, FpsAlpha);
    CameraComp->SetFieldOfView(TargetFOV);
    
    if (SpringArmComp->TargetArmLength < 20.0f)
    {
       if (GetMesh()) GetMesh()->SetOwnerNoSee(true);
    }
    else
    {
       if (GetMesh()) GetMesh()->SetOwnerNoSee(false);
    }
}

void AGuestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UGuestInputComponent* GIC = CastChecked<UGuestInputComponent>(PlayerInputComponent))
    {
       if (UInputAction* RawMove = Cast<UInputAction>(IA_Move.Get()))
       {
          GIC->BindAction(RawMove, ETriggerEvent::Triggered, this, &AGuestCharacter::MoveAction);
       }

       if (UInputAction* RawLook = Cast<UInputAction>(IA_Look.Get()))
       {
          GIC->BindAction(RawLook, ETriggerEvent::Triggered, this, &AGuestCharacter::LookAction);
       }

       if (UInputAction* RawJump = Cast<UInputAction>(IA_Jump.Get()))
       {
          GIC->BindAction(RawJump, ETriggerEvent::Started, this, &AGuestCharacter::JumpAction);
          GIC->BindAction(RawJump, ETriggerEvent::Completed, this, &ACharacter::StopJumping); 
       }

       if (InteractAction)
       {
          GIC->BindAction(InteractAction, ETriggerEvent::Started, this, &AGuestCharacter::OnInteract);
       } 

       if (IA_DigicamControl)
       {
          GIC->BindAction(IA_DigicamControl, ETriggerEvent::Triggered, this, &AGuestCharacter::DigicamControlAction);
       }

       if (IA_DigicamShutter)
       {
          GIC->BindAction(IA_DigicamShutter, ETriggerEvent::Started, this, &AGuestCharacter::DigicamShutterAction);
       }

       if (IA_DigicamToggle)
       {
          GIC->BindAction(IA_DigicamToggle, ETriggerEvent::Started, this, &AGuestCharacter::DigicamToggleAction);
       }

       if (IA_Zoom)
       {
          GIC->BindAction(IA_Zoom, ETriggerEvent::Triggered, this, &AGuestCharacter::ZoomAction);
       }

       if (IA_FreeLook)
       {
          GIC->BindAction(IA_FreeLook, ETriggerEvent::Started, this, &AGuestCharacter::FreeLookStart);
          GIC->BindAction(IA_FreeLook, ETriggerEvent::Completed, this, &AGuestCharacter::FreeLookEnd);
       }

       if (IA_Sprint)
       {
          GIC->BindAction(IA_Sprint, ETriggerEvent::Started, this, &AGuestCharacter::StartSprinting);
          GIC->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AGuestCharacter::StopSprinting);
       }

       if (IA_Crouch)
       {
          GIC->BindAction(IA_Crouch, ETriggerEvent::Started, this, &AGuestCharacter::StartCrouch);
          GIC->BindAction(IA_Crouch, ETriggerEvent::Completed, this, &AGuestCharacter::EndCrouch);
       }
       
       ensureMsgf(GAbilityInputConfigData, TEXT("캐릭터에 어빌리티 인풋 구성 데이터 할당 안됨"));
       
       GIC->BindAbilityInputAction(GAbilityInputConfigData,this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
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
    
   if (CharacterData)
   {
      TargetZoomLength -= (ZoomInput * CharacterData->ZoomStep);
      TargetZoomLength = FMath::Clamp(TargetZoomLength, CharacterData->MinZoomLength, CharacterData->MaxZoomLength);
   }
}

void AGuestCharacter::FreeLookStart(const FInputActionValue& Value)
{
    bIsFreeLooking = true;
    bUseControllerRotationYaw = false;
    G_LOG(TEXT("자유 시점 시작!")); 
}

void AGuestCharacter::FreeLookEnd(const FInputActionValue& Value)
{
    bIsFreeLooking = false;

    if (Controller != nullptr)
    {
       float CharacterYaw = GetActorRotation().Yaw;
       FRotator ResetRotation = FRotator(Controller->GetControlRotation().Pitch, CharacterYaw, 0.0f);
       Controller->SetControlRotation(ResetRotation);
    }

    G_LOG(TEXT("자유 시점 종료~"));
}
#pragma endregion

#pragma region MovementSpeed
float AGuestCharacter::GetMovementSpeed() const
{
    return GetVelocity().Size2D();
}
#pragma endregion

#pragma region Sprint
void AGuestCharacter::StartSprinting()
{
    if (GetCharacterMovement() && CharacterData)
    {
       GetCharacterMovement()->MaxWalkSpeed = CharacterData->SprintSpeed;
       UE_LOG(LogTemp, Warning, TEXT("달리기 시작: 현재 속도 %f"), CharacterData->SprintSpeed);
    }
}

void AGuestCharacter::StopSprinting()
{
    if (GetCharacterMovement() && CharacterData)
    {
       GetCharacterMovement()->MaxWalkSpeed = CharacterData->WalkSpeed;
       UE_LOG(LogTemp, Warning, TEXT("달리기 중지: 현재 속도 %f"), CharacterData->WalkSpeed);
    }
}


#pragma endregion

#pragma region Crouch
void AGuestCharacter::StartCrouch(const FInputActionValue& Value)
{
    Crouch();
    UE_LOG(LogTemp, Log, TEXT("캐릭터 조작: 앉기 상태 진입"));
}

void AGuestCharacter::EndCrouch(const FInputActionValue& Value)
{
    UnCrouch();
    UE_LOG(LogTemp, Log, TEXT("캐릭터 조작: 앉기 상태 해제"));
}
#pragma endregion

UAbilitySystemComponent* AGuestCharacter::GetAbilitySystemComponent() const
{
   return GuestAbilitySystemComponent;
}

void AGuestCharacter::AbilityInputPressed(FGameplayTag InputTag)
{
   GuestAbilitySystemComponent->OnAbilityPressed(InputTag);
}

void AGuestCharacter::AbilityInputReleased(FGameplayTag InputTag)
{
   GuestAbilitySystemComponent->OnAbilityReleased(InputTag);
}
