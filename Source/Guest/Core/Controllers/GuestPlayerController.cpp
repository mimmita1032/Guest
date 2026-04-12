// Copyright (c) 2026 Anything Left Behind?. All rights reserved.


#include "Guest/Core/Controllers/GuestPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "Guest/UI/Layout/GuestPrimaryLayout.h"
#include "Guest/Utils/GLog.h"
#include "Kismet/GameplayStatics.h"
#include "Guest/Save/GuestSaveGame.h"
#include "Guest/UI/SaveLoad/GuestSaveBoardWidget.h"
#include "Guest/UI/SaveLoad/GuestLoadBoardWidget.h"
#include "Guest/Core/GameInstance/GuestGameInstance.h"
#include "Guest/Save/GuestMapPackageUtils.h"
#include "Guest/Save/GuestSaveSlotNames.h"

namespace
{
	const FString GGuestTestSaveSlot(TEXT("GuestTestSlot"));
}


void AGuestPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	bShowMouseCursor = false;

	// ---------------------------------------------------------
	// 여기 까지는 디버그 UI인데 Stack에 영향안두게 그냥 두겠슴당
	// ---------------------------------------------------------
	
	if (PrimaryLayoutClass)
	{
		PrimaryLayoutInstance = CreateWidget<UGuestPrimaryLayout>(this, PrimaryLayoutClass);
		if (PrimaryLayoutInstance)
		{
			PrimaryLayoutInstance->AddToViewport();
			G_LOG(TEXT("Primary Layout"));
		}
	}
	else
	{
		G_ERR(TEXT("PrimaryLayoutClass가 비어있습니다! BP_GuestPlayerController의 디테일 패널을 확인하세요"));
	}
	

}


void AGuestPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (IA_ToggleDebugUI)
		{
			EIC->BindAction(IA_ToggleDebugUI, ETriggerEvent::Started, this, &AGuestPlayerController::ToggleDebugUI);
		}
		if (IA_SaveGame)
		{
			EIC->BindAction(IA_SaveGame, ETriggerEvent::Started, this, &AGuestPlayerController::ShowSaveBoard);
		}
		if (IA_LoadGame)
		{
			EIC->BindAction(IA_LoadGame, ETriggerEvent::Started, this, &AGuestPlayerController::ShowLoadBoard);
		}
	}
	
}

#pragma region DebugUI

void AGuestPlayerController::ToggleDebugUI()
{
	if (!DebugWidgetClass)
	{
		G_ERR(TEXT("DebugWidgetClass가 할당되지 않았습니다! BP_GuestPlayerController의 디테일 패널을 확인하세요."));
		return;
	}
	// 위젯이 아직 생성되지 않았다면 최초 1회 생성하여 메모리에 할당
	if (!DebugWidgetInstance && DebugWidgetClass)
	{
		DebugWidgetInstance = CreateWidget<UUserWidget>(this, DebugWidgetClass);
	}

	if (DebugWidgetInstance)
	{
		if (DebugWidgetInstance->IsInViewport())
		{
			// [끄기] 화면에서 제거하고 조작을 게임 전용으로 변경
			DebugWidgetInstance->RemoveFromParent();
			
			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);
			bShowMouseCursor = false;
			
			G_LOG(TEXT("디버그 UI 비활성화"));
		}
		else
		{
			// [켜기] 화면에 추가하고 조작을 UI 포함 모드로 변경
			DebugWidgetInstance->AddToViewport();
			
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(DebugWidgetInstance->TakeWidget());
			SetInputMode(InputMode);
			bShowMouseCursor = true;
			
			G_LOG(TEXT("디버그 UI 활성화"));
		}
	}
}
#pragma endregion

#pragma region SaveGame

bool AGuestPlayerController::SaveCurrentGameToSlot(const FString& SlotName, int32 UserIndex)
{
	UGuestSaveGame* SaveObject = Cast<UGuestSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UGuestSaveGame::StaticClass()));

	if (!SaveObject)
	{
		return false;
	}

	APawn* ControllerPawn = GetPawn();
	if (!ControllerPawn)
	{
		return false;
	}

	if (UWorld* World = GetWorld())
	{
		const FString RawPackage = World->PersistentLevel
			? World->PersistentLevel->GetOutermost()->GetName()
			: FString();
		SaveObject->MapPackageName = GuestMapPackage::StripPIEFromPackagePath(RawPackage);
	}

	SaveObject->SaveVersion = 2;
	SaveObject->SavedAt = FDateTime::Now();
	SaveObject->PlayerWorld.Location = ControllerPawn->GetActorLocation();
	SaveObject->PlayerWorld.Rotation = ControllerPawn->GetActorRotation();

	const bool bOk = UGameplayStatics::SaveGameToSlot(SaveObject, SlotName, UserIndex);

	if (bOk)
	{
		G_LOG(TEXT("저장 성공: %s"), *SlotName);
	}
	else
	{
		G_LOG(TEXT("저장 실패: %s"), *SlotName);
	}

	return bOk;
}

void AGuestPlayerController::OnSaveGamePressed(const FInputActionValue& Value)
{
	SaveCurrentGameToSlot(GGuestTestSaveSlot, GuestSaveSlots::DefaultUserIndex());
}

void AGuestPlayerController::OnLoadGamePressed(const FInputActionValue& Value)
{
	if (UGuestGameInstance* GI = Cast<UGuestGameInstance>(GetGameInstance()))
	{
		GI->RequestLoadFromSlot(GGuestTestSaveSlot, GuestSaveSlots::DefaultUserIndex());
	}
}

void AGuestPlayerController::ShowSaveBoard()
{
	G_LOG(TEXT("Show save"))
	if (SaveBoardClass)
	{
		SaveBoardWidget = CreateWidget<UGuestSaveBoardWidget>(this, SaveBoardClass);
	}
	
	if (SaveBoardWidget)
	{
		SaveBoardWidget->AddToViewport();
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(SaveBoardWidget->TakeWidget());
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
}

void AGuestPlayerController::ShowLoadBoard()
{
	G_LOG(TEXT("Show Load"))
	if (LoadBoardClass)
	{
		LoadBoardWidget = CreateWidget<UGuestLoadBoardWidget>(this, LoadBoardClass);
	}
	if (LoadBoardWidget)
	{
		LoadBoardWidget->AddToViewport();
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(LoadBoardWidget->TakeWidget());
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
}

#pragma endregion
