// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "GuestPlayerController.generated.h"

class UGuestUISubsystem;
class UGuestPrimaryLayout;
class UGuestGameInstance;

/**
 * AGuestPlayerController
 * * UI 프레임워크와 연동되어 최상위 레이아웃(Primary Layout)을 생성하고 관리합니다.
 */
UCLASS()
class GUEST_API AGuestPlayerController : public APlayerController
{
    GENERATED_BODY()

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

public:

	AGuestPlayerController();
	
	/** UI 서브시스템에 빠르게 접근하기 위한 헬퍼 */
	UFUNCTION(BlueprintPure, Category = "Guest|UI")
	UGuestUISubsystem* GetUISubsystem() const;

	/** 현재 활성화된 최상위 레이아웃 반환 */
	UFUNCTION(BlueprintPure, Category = "Guest|UI")
	UGuestPrimaryLayout* GetPrimaryLayout() const { return PrimaryLayout; }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Guest|UI")
	TSubclassOf<UGuestPrimaryLayout> PrimaryLayoutClass;

	UPROPERTY()
	TObjectPtr<UGuestPrimaryLayout> PrimaryLayoutInstance;

	virtual void CreatePrimaryLayout();

private:
	/** 생성된 최상위 레이아웃 인스턴스 보관 */
	UPROPERTY()
	TObjectPtr<UGuestPrimaryLayout> PrimaryLayout;

	
#pragma endregion
/*===========================================================
 * [디버그 전용] 퀘스트 시스템 콘솔 테스트 함수
 *
 * ※ 사용법: 에디터 실행 중 콘솔(~ 키)을 열고 아래 명령어 입력
 *
 *  DebugAcceptQuest Q_Main_001
 *    → QuestID가 Q_Main_001인 퀘스트를 조건 검증 후 강제 수락
 *
 *  DebugUpdateObjective Food0 1
 *    → TargetID=Food0 목표를 1만큼 강제 진행 (아이템 직접 줍지 않아도 됨)
 *
 *  DebugQuestStatus
 *    → 현재 진행 중인 퀘스트 목록을 로그로 출력
 *
 *  DebugCompletedQuests
 *    → 완료된 퀘스트 목록을 로그로 출력
 *
 * ※ Exec 함수는 에디터/개발 빌드에서만 동작하며 릴리즈 빌드에서는 무시됨
 *===========================================================*/
#pragma region QuestDebug
public:
	// [디버그] 퀘스트 강제 수락 — 콘솔 입력: DebugAcceptQuest Q_Main_001
	UFUNCTION(Exec)
	void DebugAcceptQuest(FName QuestID);

	// [디버그] 퀘스트 목표 강제 갱신 — 콘솔 입력: DebugUpdateObjective Food0 1
	UFUNCTION(Exec)
	void DebugUpdateObjective(FName TargetID, int32 Amount);

	// [디버그] 진행 중인 퀘스트 목록 로그 출력 — 콘솔 입력: DebugQuestStatus
	UFUNCTION(Exec)
	void DebugQuestStatus();

	// [디버그] 완료된 퀘스트 목록 로그 출력 — 콘솔 입력: DebugCompletedQuests
	UFUNCTION(Exec)
	void DebugCompletedQuests();
#pragma endregion

#pragma region SaveDebug

public:
	/** 현재 맵·폰 위치/회전을 주어진 슬롯에 저장 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool SaveCurrentGameToSlot(const FString& SlotName, int32 UserIndex = 0);

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
