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
			EIC->BindAction(IA_SaveGame, ETriggerEvent::Started, this, &AGuestPlayerController::OnSaveGamePressed);
		}
		if (IA_LoadGame)
		{
			EIC->BindAction(IA_LoadGame, ETriggerEvent::Started, this, &AGuestPlayerController::OnLoadGamePressed);
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
void AGuestPlayerController::OnSaveGamePressed(const FInputActionValue& Value)
{
	UGuestSaveGame* SaveObject = Cast<UGuestSaveGame>(
	UGameplayStatics::CreateSaveGameObject(UGuestSaveGame::StaticClass()));
	
	if (!SaveObject) return;
	
	APawn* ControllerPawn = GetPawn();
	if (!ControllerPawn) return;
	
	SaveObject -> SaveVersion = 1;
	SaveObject -> PlayerWorld.Location = ControllerPawn -> GetActorLocation();
	SaveObject -> PlayerWorld.Rotation = ControllerPawn -> GetActorRotation();
	
	const bool bOk = UGameplayStatics::SaveGameToSlot(SaveObject, GGuestTestSaveSlot, 0);
	
	if (bOk)
	{
		G_LOG(TEXT("저장 성공"));
	}else
	{
		G_LOG(TEXT("저장 실패"));
	}
}

void AGuestPlayerController::OnLoadGamePressed(const FInputActionValue& Value)
{
	if (!UGameplayStatics::DoesSaveGameExist(GGuestTestSaveSlot,0))
	{
		return;
	}
	
	USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(GGuestTestSaveSlot,0);
	UGuestSaveGame* SaveObject = Cast<UGuestSaveGame>(Loaded);
	if (!SaveObject) return;
	
	APawn* ControllerPawn = GetPawn();
	if (!ControllerPawn) return;
	
	ControllerPawn -> SetActorLocation(SaveObject->PlayerWorld.Location,false,nullptr, ETeleportType::TeleportPhysics);
	ControllerPawn -> SetActorRotation(SaveObject->PlayerWorld.Rotation,ETeleportType::TeleportPhysics);
}
#pragma endregion
