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
#include "Guest/UI/GameplayTags/GuestGameplayTags.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/UI/Settings/GuestUISettings.h"



namespace
{
	const FString GGuestTestSaveSlot(TEXT("GuestTestSlot"));
}

AGuestPlayerController::AGuestPlayerController()
{
}

void AGuestPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 플레이어인 경우에만 UI 레이아웃 생성
	if (IsLocalController())
	{
		CreatePrimaryLayout();
	}
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	if (UGuestUISubsystem* UISys = GetGameInstance()->GetSubsystem<UGuestUISubsystem>())
	{
		UISys->PushWidget(GuestGameplayTags::TAG_WidgetStack_GameHUD, GuestGameplayTags::TAG_Widget_GameHUD);
		UE_LOG(LogTemp, Log, TEXT("UI 시스템: 메인 화면 로드 요청 완료"));
	}
	
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

// 디버그 및 테스트용 UI (Common UI 미사용)
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
		// IA_ToggleInventory는 AGuestCharacter::SetupPlayerInputComponent에서 바인딩
	}
}

#pragma region PrimaryLayout

void AGuestPlayerController::CreatePrimaryLayout()
{
	// 1. UI 설정 로드
	const UGuestUISettings* UISettings = GetDefault<UGuestUISettings>();
	if (!UISettings) return;

	// 2. Primary Layout 클래스 소프트 레퍼런스 확인
	TSoftClassPtr<UUserWidget> LayoutClassPtr = UISettings->PrimaryLayoutClass;
	if (LayoutClassPtr.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("[GuestUI] GuestUISettings에 PrimaryLayoutClass가 설정되지 않았습니다!"));
		return;
	}

	// 3. 클래스 동기 로드 (최상위 레이아웃은 게임 시작 시 즉시 필요하므로 동기 로드)
	UClass* LayoutClass = LayoutClassPtr.LoadSynchronous();
	if (!LayoutClass) return;

	// 4. 위젯 생성 및 뷰포트 추가
	PrimaryLayout = CreateWidget<UGuestPrimaryLayout>(this, LayoutClass);
	if (PrimaryLayout)
	{
		PrimaryLayout->AddToViewport(0); // 가장 낮은 ZOrder로 시작 (내부 스택들이 그 위를 덮음)
		
		// 참고: PrimaryLayout->NativeOnInitialized() 내부에서 
		// 각 Stack들을 Subsystem에 RegisterStack하게 됩니다.
		UE_LOG(LogTemp, Log, TEXT("[GuestUI] Primary Layout 생성 및 뷰포트 추가 완료."));
	}
}


UGuestUISubsystem* AGuestPlayerController::GetUISubsystem() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		return GI->GetSubsystem<UGuestUISubsystem>();
	}
	return nullptr;
}
#pragma endregion

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

#pragma region QuestDebug

// 헬퍼: GQuestSubsystem 조회 (실패 시 로그 출력 후 nullptr 반환)
static UGQuestSubsystem* GetQuestSys(APlayerController* PC)
{
	UGQuestSubsystem* QuestSys = PC->GetGameInstance()->GetSubsystem<UGQuestSubsystem>();
	if (!QuestSys)
	{
		UE_LOG(LogGSystem, Error, TEXT("[디버그] GQuestSubsystem을 찾을 수 없습니다."));
	}
	return QuestSys;
}

/*
 * 콘솔 명령어: DebugAcceptQuest Q_Main_001
 * 선행 퀘스트·시간 조건 검증을 포함한 정상 수락 흐름을 그대로 탑니다.
 * DataTable에 해당 QuestID Row가 없으면 경고 로그가 출력됩니다.
 */
void AGuestPlayerController::DebugAcceptQuest(FName QuestID)
{
	UGQuestSubsystem* QuestSys = GetQuestSys(this);
	if (!QuestSys) return;

	G_LOG(TEXT("[디버그] 퀘스트 강제 수락 시도: %s"), *QuestID.ToString());
	QuestSys->AcceptQuest(QuestID);
}

/*
 * 콘솔 명령어: DebugUpdateObjective Food0 1
 * 아이템을 직접 줍지 않아도 델리게이트를 강제 Broadcast해 목표 진행도를 올립니다.
 * TargetID는 DataTable Objectives의 TargetID 값과 정확히 일치해야 합니다.
 */
void AGuestPlayerController::DebugUpdateObjective(FName TargetID, int32 Amount)
{
	UGQuestSubsystem* QuestSys = GetQuestSys(this);
	if (!QuestSys) return;

	G_LOG(TEXT("[디버그] 목표 강제 갱신: TargetID=%s, Amount=%d"), *TargetID.ToString(), Amount);
	QuestSys->OnObjectiveUpdated.Broadcast(TargetID, Amount);
}

/*
 * 콘솔 명령어: DebugQuestStatus
 * 현재 진행 중인 퀘스트 ID 목록을 로그로 출력합니다.
 * 진행 중인 퀘스트가 없으면 "없음"을 출력합니다.
 */
void AGuestPlayerController::DebugQuestStatus()
{
	UGQuestSubsystem* QuestSys = GetQuestSys(this);
	if (!QuestSys) return;

	TArray<FName> ActiveIDs = QuestSys->GetActiveQuestIDs();
	if (ActiveIDs.IsEmpty())
	{
		G_LOG(TEXT("[디버그] 진행 중인 퀘스트: 없음"));
		return;
	}

	G_LOG(TEXT("[디버그] 진행 중인 퀘스트 (%d개):"), ActiveIDs.Num());
	for (const FName& ID : ActiveIDs)
	{
		G_LOG(TEXT("[디버그]  - %s"), *ID.ToString());
	}
}

/*
 * 콘솔 명령어: DebugCompletedQuests
 * 완료된 퀘스트 ID 목록을 로그로 출력합니다.
 * 완료된 퀘스트가 없으면 "없음"을 출력합니다.
 */
void AGuestPlayerController::DebugCompletedQuests()
{
	UGQuestSubsystem* QuestSys = GetQuestSys(this);
	if (!QuestSys) return;

	TArray<FName> CompletedIDs = QuestSys->GetCompletedQuestIDs();
	if (CompletedIDs.IsEmpty())
	{
		G_LOG(TEXT("[디버그] 완료된 퀘스트: 없음"));
		return;
	}

	G_LOG(TEXT("[디버그] 완료된 퀘스트 (%d개):"), CompletedIDs.Num());
	for (const FName& ID : CompletedIDs)
	{
		G_LOG(TEXT("[디버그]  - %s"), *ID.ToString());
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

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGQuestSubsystem* QuestSys = GI->GetSubsystem<UGQuestSubsystem>())
		{
			QuestSys->ExportQuestSaveData(
				SaveObject->SavedActiveQuests,
				SaveObject->SavedCompletedQuestIDs);
		}
	}
	
	SaveObject->SaveVersion = 3;
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
