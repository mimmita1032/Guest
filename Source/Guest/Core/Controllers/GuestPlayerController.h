// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "GuestPlayerController.generated.h"

/**
 * 
 */

class UGuestPrimaryLayout;
class UGuestGameInstance;

UCLASS()
class GUEST_API AGuestPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** 현재 맵·폰 위치/회전을 주어진 슬롯에 저장 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool SaveCurrentGameToSlot(const FString& SlotName, int32 UserIndex = 0);

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

#pragma region DebugUI
protected:
	// WBP_DebugTime
	UPROPERTY(EditAnywhere, Category = "Debug|UI")
	TSubclassOf<class UUserWidget> DebugWidgetClass;

	// F1 키 에 바인딩할 입력 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> IA_ToggleDebugUI;

	// 화면에 띄울 위젯의 인스턴스를 보관
	UPROPERTY()
	TObjectPtr<class UUserWidget> DebugWidgetInstance;

	// 입력이 들어왔을 때 실행
	void ToggleDebugUI();
#pragma endregion
#pragma region CommonUI

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Guest|UI")
	TSubclassOf<UGuestPrimaryLayout> PrimaryLayoutClass;

	UPROPERTY()
	TObjectPtr<UGuestPrimaryLayout> PrimaryLayoutInstance;
	
#pragma endregion
#pragma region SaveDebug


protected:
	
	UPROPERTY(BlueprintReadOnly, Category = "Save|UI")
	class UGuestSaveBoardWidget* SaveBoardWidget;
	
	UPROPERTY(BlueprintReadOnly, Category = "Save|UI")
	class UGuestLoadBoardWidget* LoadBoardWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Save|UI")
	TSubclassOf<class UGuestSaveBoardWidget> SaveBoardClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Save|UI")
	TSubclassOf<class UGuestLoadBoardWidget> LoadBoardClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> IA_SaveGame;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> IA_LoadGame;
	
	void OnSaveGamePressed(const FInputActionValue& Value);
	void OnLoadGamePressed(const FInputActionValue& Value);
	void ShowSaveBoard();
	void ShowLoadBoard();
	
#pragma endregion
};
