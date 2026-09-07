// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GuestCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UGDigicamComponent;
class UGCameraComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UGCharacterDataAsset;
class UGCharacterGASData;
class UGInputConfigData;
class UGInventoryComponent;
class UGItemPlacementComponent;
class UGuestPawnUIComponent;

UCLASS()
class GUEST_API AGuestCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AGuestCharacter();

protected:
    virtual void BeginPlay() override;
    
    virtual void PossessedBy(AController* NewController) override;

    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void MoveAction(const FInputActionValue& Value);
    void LookAction(const FInputActionValue& Value);
    void JumpAction(const FInputActionValue& Value);
    void StartCrouch(const FInputActionValue& Value);
    void EndCrouch(const FInputActionValue& Value);
    void ZoomAction(const FInputActionValue& Value);
    void FreeLookStart(const FInputActionValue& Value);
    void FreeLookEnd(const FInputActionValue& Value);
    void ToggleInventoryAction(const FInputActionValue& Value);
    void ToggleSaveBoardAction(const FInputActionValue& Value);
    void ToggleLoadBoardAction(const FInputActionValue& Value);

    // 기획 데이터 에셋 참조
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    TObjectPtr<UGCharacterDataAsset> CharacterData;

protected:
    //입력 구성 데이터
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UGInputConfigData> GAbilityInputConfigData;
    //입력
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Move;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Look;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Jump;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Crouch;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_ToggleInventory;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_SaveGame;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_LoadGame;

    // 아이템 배치 모드 확정/취소 — UMG 위젯 히트테스트에 기대지 않고 Enhanced Input으로 직접 처리
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_ConfirmPlacement;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_CancelPlacement;

    void ConfirmPlacementAction(const FInputActionValue& Value);
    void CancelPlacementAction(const FInputActionValue& Value);

    //카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> CameraComp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Zoom;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_FreeLook;

    //상호작용
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    TObjectPtr<class UGInteractionComponent> InteractionComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digicam")
    TObjectPtr<UGDigicamComponent> DigicamComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digicam")
    TObjectPtr<UGCameraComponent> CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digicam")
    TObjectPtr<USceneCaptureComponent2D> PhotoCaptureComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Digicam")
    TObjectPtr<UTextureRenderTarget2D> PhotoRenderTarget;

#pragma region Digicam Functions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_DigicamControl;

    // 셔터 — 촬영 전용
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_DigicamShutter;

    // 시공간 이동 확정 — 셔터와 분리된 별도 입력.
    // 되돌릴 수 없는 행동이라 실수로 눌리지 않도록 촬영과 키를 나눈다
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_DigicamTravel;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_DigicamToggle;

    // 입력 처리 함수
    UFUNCTION()
    void DigicamControlAction(const FInputActionValue& Value);

    UFUNCTION()
    void DigicamShutterAction(const FInputActionValue& Value);

    UFUNCTION()
    void DigicamTravelAction(const FInputActionValue& Value);

    UFUNCTION()
    void DigicamToggleAction(const FInputActionValue& Value);
#pragma endregion

#pragma region CameraZoom
    // 연산용 줌 타겟
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
    float TargetZoomLength;

    // 연산용 줌 속도
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
    float ZoomSpeed;

    // FOV/소켓오프셋 전환 연출용으로 TargetZoomLength를 부드럽게 뒤따라가는 값
    // (TargetZoomLength는 휠 한 번에 ZoomStep만큼 즉시 점프하므로, FOV 등에 그대로 쓰면 뚝뚝 끊겨 보임)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
    float SmoothedZoomLength;

    // 아래를 내려다볼 때 SpringArm이 바닥 콜리전에 걸리지 않도록 줄어드는 최소 길이
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
    float MinArmLengthWhenLookingDown = 150.0f;

    // 위를 올려다볼 때(피치가 MaxViewPitch에 가까워질 때) 카메라가 지면 밑으로 내려가지
    // 않도록 줄어드는 최소 길이. 반직관적이지만 이 값을 "줄일수록" 카메라가 덜 파묻힌다 —
    // 위를 볼 때 카메라 위치는 sin(피치)×팔길이만큼 아래로 내려가므로, 팔이 길수록(=이 값이
    // 클수록) 더 깊이 내려간다. Lyra의 TargetOffsetCurve처럼 결국 눈으로 보고 정하는 값이라
    // EditAnywhere로 노출해뒀다 — 에디터에서 조정 가능
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
    float MinArmLengthWhenLookingUp = 180.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|State")
    bool bIsFreeLooking = false;
#pragma endregion // CameraZoom

#pragma region CameraPitch
public:
    // AGuestPlayerController::UpdateRotation에서 ControlRotation 클램프에 사용
    UFUNCTION(BlueprintPure, Category = "Camera|Pitch")
    float GetMinViewPitch() const { return MinViewPitch; }

    UFUNCTION(BlueprintPure, Category = "Camera|Pitch")
    float GetMaxViewPitch() const { return MaxViewPitch; }

protected:
    // 카메라 피치 제한 (바닥 회피 계산 및 ControlRotation 클램프에 사용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Pitch")
    float MinViewPitch = -25.0f;

    // 원래 25도였다. 40도로 보수적으로 늘림 — MinArmLengthWhenLookingUp이 300이라
    // 팔이 완만하게만 당겨지므로, 그 이상 넓히면 다시 지면에 파묻힐 수 있다.
    // 더 넓히고 싶으면 이 값과 MinArmLengthWhenLookingUp을 같이 에디터에서 보며 조정할 것.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Pitch")
    float MaxViewPitch = 40.0f;
#pragma endregion // CameraPitch

#pragma region anim
public:
    UFUNCTION(BlueprintPure, Category = "Character|Animation")
    float GetMovementSpeed() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|State")
    bool bIsCrouching = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|State")
    bool bIsAiming = false;
#pragma endregion

#pragma region Sprint
protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Sprint;

    void StartSprinting();
    void StopSprinting();
#pragma endregion // Sprint

#pragma region Inventory
public:
    UFUNCTION(BlueprintPure, Category = "Inventory")
    UGInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    UGItemPlacementComponent* GetItemPlacementComponent() const { return ItemPlacementComponent; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UGInventoryComponent> InventoryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UGItemPlacementComponent> ItemPlacementComponent;
#pragma endregion

#pragma region PawnUI
public:
    UFUNCTION(BlueprintPure, Category = "UI")
    UGuestPawnUIComponent* GetPawnUIComponent() const { return PawnUIComponent; }

private:
    UPROPERTY(VisibleAnywhere, Category = "UI")
    TObjectPtr<UGuestPawnUIComponent> PawnUIComponent;
#pragma endregion

#pragma region Death
private:
    UFUNCTION()
    void OnDeadTagChanged(const FGameplayTag Tag, int32 NewCount);

    void HandleDeath();

    /** 사망 후 주점으로 돌려보낸다. HandleDeath가 타이머로 부른다. */
    void ReturnToTavern();

    /** 죽은 것을 볼 시간. 이 뒤에 주점으로 넘어간다. */
    UPROPERTY(EditDefaultsOnly, Category = "Death", meta = (ClampMin = "0.0", Units = "s"))
    float DeathReturnDelay = 2.0f;

    /** 복귀할 주점 좌표. DT_SpacetimeData의 Tavern 행과 맞춰야 한다. */
    UPROPERTY(EditDefaultsOnly, Category = "Death")
    int32 TavernYear = 2026;

    UPROPERTY(EditDefaultsOnly, Category = "Death")
    int32 TavernAreaCode = 0;
#pragma endregion
    
    ///     GAS     ///

public:
    // IAbilitySystemInterface begin
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    // IAbilitySystemInterface end
private:
    UPROPERTY(VisibleAnywhere, Category = "GAS")
    class UGuestAbilitySystemComponent* GuestAbilitySystemComponent;
    
    UPROPERTY(VisibleAnywhere, Category = "GAS")
    class UGuestAttributeSet* GuestAttributeSet;
    
    void AbilityInputPressed(FGameplayTag InputTag);
    void AbilityInputReleased(FGameplayTag InputTag);
    
    //Gas 데이터 에셋
    UPROPERTY(EditDefaultsOnly, Category = "Data")
    TSoftObjectPtr<UGCharacterGASData> CharacterGasData;
};
