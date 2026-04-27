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
    void OnInteract(const struct FInputActionValue& Value);
    void ZoomAction(const FInputActionValue& Value);
    void FreeLookStart(const FInputActionValue& Value);
    void FreeLookEnd(const FInputActionValue& Value);
    void ToggleInventoryAction(const FInputActionValue& Value);

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<class UInputAction> InteractAction;

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_DigicamShutter;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_DigicamToggle;

    // 입력 처리 함수
    UFUNCTION()
    void DigicamControlAction(const FInputActionValue& Value);

    UFUNCTION()
    void DigicamShutterAction(const FInputActionValue& Value);

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|State")
    bool bIsFreeLooking = false;
#pragma endregion // CameraZoom

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
private:
    bool bIsInventoryOpen = false;
    bool bIsDigicamOpen = false;
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