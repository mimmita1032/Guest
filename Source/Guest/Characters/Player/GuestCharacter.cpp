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
#include "Guest/Components/GCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
// 데이터 에셋 헤더 추가
#include "Guest/Data/DataAssets/GCharacterDataAsset.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
//gas
#include "Guest/GAS/GuestAbilitySystemComponent.h"
#include "Guest/GAS/GuestAttributeSet.h"
//gas DATA
#include "Guest/DATA/DataAssets/GCharacterGASData.h"
//인풋 구성
#include "Guest/Data/Input/GInputConfigData.h"
#include "Guest/Components/Input/GuestInputComponent.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Components/CharacterComponents/GItemPlacementComponent.h"
#include "Guest/Components/CharacterComponents/GuestPawnUIComponent.h"

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
    SpringArmComp->ProbeSize = 20.0f;
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

    InventoryComponent   = CreateDefaultSubobject<UGInventoryComponent>(TEXT("InventoryComponent"));
    ItemPlacementComponent = CreateDefaultSubobject<UGItemPlacementComponent>(TEXT("ItemPlacementComponent"));
    InteractionComponent = CreateDefaultSubobject<UGInteractionComponent>(TEXT("InteractionComponent"));
    DigicamComponent = CreateDefaultSubobject<UGDigicamComponent>(TEXT("DigicamComponent"));
    CameraComponent = CreateDefaultSubobject<UGCameraComponent>(TEXT("CameraComponent"));
    PhotoCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("PhotoCaptureComponent"));
    PhotoCaptureComponent->SetupAttachment(CameraComp);
    PhotoCaptureComponent->bCaptureEveryFrame = false;
    PhotoCaptureComponent->bCaptureOnMovement = false;
    
    // 연산용 초기값
    TargetZoomLength = 400.0f;
    ZoomSpeed = 10.0f;
    SmoothedZoomLength = TargetZoomLength;
   
   GuestAbilitySystemComponent = CreateDefaultSubobject<UGuestAbilitySystemComponent>(TEXT("GuestAbilitySystemComponent"));
   GuestAttributeSet = CreateDefaultSubobject<UGuestAttributeSet>(TEXT("GuestAttributeSet"));
   PawnUIComponent = CreateDefaultSubobject<UGuestPawnUIComponent>(TEXT("PawnUIComponent"));
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

    if (CameraComponent && PhotoCaptureComponent && PhotoRenderTarget)
    {
        CameraComponent->SetupCapture(PhotoCaptureComponent, PhotoRenderTarget);
    }
    
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
       // 피치 제한은 SpringArm이 읽는 ControlRotation에 직접 걸어야 해서 LookAction에서 클램프함
       // (PlayerCameraManager->ViewPitchMin/Max는 이 카메라 구조에 영향 없음)
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
      SmoothedZoomLength = TargetZoomLength;

      UE_LOG(LogTemp, Log, TEXT("캐릭터 데이터 에셋 로드 성공. 점프력: %f"), CharacterData->JumpZVelocity);
   }

    SpringArmComp->TargetArmLength = TargetZoomLength;
}

void AGuestCharacter::PossessedBy(AController* NewController)
{
   Super::PossessedBy(NewController);

   if (GuestAbilitySystemComponent)
   {
      GuestAbilitySystemComponent->InitAbilityActorInfo(this, this);
   }

   ensureMsgf(!CharacterGasData.IsNull(), TEXT("캐릭터 GAS 데이터 할당 안됨"));

   if (UGCharacterGASData* LoadedData = CharacterGasData.LoadSynchronous())
   {
      LoadedData->GiveToASC(GuestAbilitySystemComponent);
   }

   // GiveToASC 이후 — Attribute 초기값이 설정된 뒤 바인딩
   if (PawnUIComponent)
   {
      PawnUIComponent->InitializeWithASC(GuestAbilitySystemComponent);
   }

   // Dead Tag 이벤트 등록 — 사망 감지
   if (GuestAbilitySystemComponent)
   {
      GuestAbilitySystemComponent->RegisterGameplayTagEvent(
         GuestGameplayTags::TAG_State_Dead,
         EGameplayTagEventType::NewOrRemoved)
         .AddUObject(this, &AGuestCharacter::OnDeadTagChanged);
   }
}

void AGuestCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 수평(0도)에서 MinViewPitch(제일 아래)로 내려다볼수록 목표 길이를 점점 줄여서,
    // SpringArm이 바닥에 닿아 콜리전 트레이스로 확 당겨지는 것을 미리 방지
    const float ControlPitch = FRotator::NormalizeAxis(GetControlRotation().Pitch);
    const float PitchAlpha = FMath::GetMappedRangeValueClamped(
       FVector2D(0.0f, MinViewPitch), FVector2D(1.0f, 0.0f), ControlPitch);
    const float PitchAdjustedTargetLength = FMath::Lerp(MinArmLengthWhenLookingDown, TargetZoomLength, PitchAlpha);

    if (!FMath::IsNearlyEqual(SpringArmComp->TargetArmLength, PitchAdjustedTargetLength, 0.1f))
    {
       SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, PitchAdjustedTargetLength, DeltaTime, ZoomSpeed);
    }

    // FPS 전환 연출(소켓 오프셋/FOV/메시 숨김)은 플레이어가 실제로 의도한 줌 거리(TargetZoomLength) 기준으로
    // 판단하되, TargetZoomLength 자체는 휠 한 번에 ZoomStep만큼 즉시 점프하므로 그대로 쓰면 뚝뚝 끊겨 보임.
    // 부드럽게 뒤따라가는 SmoothedZoomLength를 통해서만 계산.
    SmoothedZoomLength = FMath::FInterpTo(SmoothedZoomLength, TargetZoomLength, DeltaTime, ZoomSpeed);
    float FpsAlpha = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 450.0f), FVector2D(0.0f, 1.0f), SmoothedZoomLength);

    SpringArmComp->SocketOffset.Y = FMath::Lerp(0.0f, 75.0f, FpsAlpha);
    SpringArmComp->SocketOffset.Z = FMath::Lerp(0.0f, 140.0f, FpsAlpha);

    if (SmoothedZoomLength < 50.0f)
    {
       SpringArmComp->bDoCollisionTest = false;
    }
    else
    {
       SpringArmComp->bDoCollisionTest = true;
    }

    float TargetFOV = FMath::Lerp(100.0f, 90.0f, FpsAlpha);
    CameraComp->SetFieldOfView(TargetFOV);

    if (SmoothedZoomLength < 20.0f)
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
       
       if (IA_ToggleInventory)
       {
          GIC->BindAction(IA_ToggleInventory, ETriggerEvent::Started, this, &AGuestCharacter::ToggleInventoryAction);
       }

       if (IA_SaveGame)
       {
          GIC->BindAction(IA_SaveGame, ETriggerEvent::Started, this, &AGuestCharacter::ToggleSaveBoardAction);
       }

       if (IA_LoadGame)
       {
          GIC->BindAction(IA_LoadGame, ETriggerEvent::Started, this, &AGuestCharacter::ToggleLoadBoardAction);
       }

       if (IA_ConfirmPlacement)
       {
          GIC->BindAction(IA_ConfirmPlacement, ETriggerEvent::Started, this, &AGuestCharacter::ConfirmPlacementAction);
       }

       if (IA_CancelPlacement)
       {
          GIC->BindAction(IA_CancelPlacement, ETriggerEvent::Started, this, &AGuestCharacter::CancelPlacementAction);
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
       // 피치 클램프는 AGuestPlayerController::UpdateRotation에서 처리.
       // AddControllerPitchInput은 즉시 반영이 아니라 RotationInput에 누적됐다가
       // 컨트롤러의 UpdateRotation에서 ControlRotation에 반영되므로, 여기서 바로
       // GetControlRotation()을 읽어 클램프해봐야 그 프레임엔 적용 전 값이라 무의미함.
    }
}

void AGuestCharacter::JumpAction(const FInputActionValue& Value)
{
    Jump();
    UE_LOG(LogTemp, Log, TEXT("점프 실행"));
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
    if (UGuestUISubsystem* UISubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGuestUISubsystem>())
    {
        if (UISubsystem->IsWidgetActive(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_Digicam))
        {
            UISubsystem->PopWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu);
            DigicamComponent->DeactivateDigicam();
            G_LOG(TEXT("디지캠 닫기"));
        }
        else
        {
            UISubsystem->PushWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_Digicam);
            DigicamComponent->ActivateDigicam();
            G_LOG(TEXT("디지캠 열기"));
        }
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
#pragma region Inventory
void AGuestCharacter::ToggleInventoryAction(const FInputActionValue& Value)
{
   if (UGuestUISubsystem* UISubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGuestUISubsystem>())
   {
      if (UISubsystem->IsWidgetActive(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_Inventory))
      {
         UISubsystem->PopWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu);

         G_LOG(TEXT("캐릭터 입력: 인벤토리 닫기 실행"));
      }
      else
      {
         UISubsystem->PushWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_Inventory);

         G_LOG(TEXT("캐릭터 입력: 인벤토리 열기 실행"));
      }
   }
}

void AGuestCharacter::ToggleSaveBoardAction(const FInputActionValue& Value)
{
   if (UGuestUISubsystem* UISubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGuestUISubsystem>())
   {
      if (UISubsystem->IsWidgetActive(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_SaveBoard))
      {
         UISubsystem->PopWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu);
         G_LOG(TEXT("캐릭터 입력: 세이브 보드 닫기 실행"));
         return;
      }

      if (UISubsystem->IsWidgetActive(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_LoadBoard))
      {
         UISubsystem->PopWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu);
      }

      UISubsystem->PushWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_SaveBoard);
      G_LOG(TEXT("캐릭터 입력: 세이브 보드 열기 실행"));
   }
}

void AGuestCharacter::ToggleLoadBoardAction(const FInputActionValue& Value)
{
   if (UGuestUISubsystem* UISubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGuestUISubsystem>())
   {
      if (UISubsystem->IsWidgetActive(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_LoadBoard))
      {
         UISubsystem->PopWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu);
         G_LOG(TEXT("캐릭터 입력: 로드 보드 닫기 실행"));
         return;
      }

      if (UISubsystem->IsWidgetActive(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_SaveBoard))
      {
         UISubsystem->PopWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu);
      }

      UISubsystem->PushWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_LoadBoard);
      G_LOG(TEXT("캐릭터 입력: 로드 보드 열기 실행"));
   }
}

void AGuestCharacter::ConfirmPlacementAction(const FInputActionValue& Value)
{
   if (ItemPlacementComponent && ItemPlacementComponent->IsPlacementActive())
   {
      ItemPlacementComponent->ConfirmPlacement();
   }
}

void AGuestCharacter::CancelPlacementAction(const FInputActionValue& Value)
{
   if (ItemPlacementComponent && ItemPlacementComponent->IsPlacementActive())
   {
      ItemPlacementComponent->CancelPlacement();
   }
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

void AGuestCharacter::OnDeadTagChanged(const FGameplayTag Tag, int32 NewCount)
{
   // NewOrRemoved: Count 0→1 시 한 번만 발화
   if (NewCount > 0)
   {
      HandleDeath();
   }
}

void AGuestCharacter::HandleDeath()
{
   // 이동 / 입력 정지
   GetCharacterMovement()->DisableMovement();
   if (APlayerController* PC = Cast<APlayerController>(GetController()))
   {
      DisableInput(PC);
   }

   // 콜리전 비활성화
   SetActorEnableCollision(false);

   // 사망 몽타주 / GameOver UI는 Blueprint에서 처리
}
