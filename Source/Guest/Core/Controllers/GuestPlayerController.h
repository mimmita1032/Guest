// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "GuestPlayerController.generated.h"

class UGuestUISubsystem;
class UGuestPrimaryLayout;
class UGuestGameInstance;
class UInputAction;
class UGQuestTrackerWidget;

/**
 * AGuestPlayerController
 * * UI 프레임워크와 연동되어 최상위 레이아웃(Primary Layout)을 생성하고 관리합니다.
 */
UCLASS()
class GUEST_API AGuestPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	// SpringArm이 ControlRotation을 직접 읽는 구조라, 여기서 최종 값을 클램프해야
	// (AddControllerPitchInput 직후 읽으면 아직 반영 전이라 무의미함)
	virtual void UpdateRotation(float DeltaTime) override;

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

	// IGenericTeamAgentInterface — Player Team 반환
	virtual FGenericTeamId GetGenericTeamId() const override;

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

#pragma region QuestTracker
public:
	UPROPERTY(EditDefaultsOnly, Category = "Guest|UI")
	TSubclassOf<UGQuestTrackerWidget> QuestTrackerClass;

private:
	UPROPERTY()
	TObjectPtr<UGQuestTrackerWidget> QuestTrackerInstance;
#pragma endregion
/*===========================================================
 * [디버그 전용] 퀘스트 시스템 콘솔 테스트 함수
 *
 * ※ 사용법: 에디터 실행 중 콘솔(~ 키)을 열고 아래 명령어 입력
 *
 *  DebugSkipToQuest Q_Smith_003
 *    → 선행 퀘스트를 거슬러 올라가 전부 완료 처리한 뒤 목표 퀘스트를 수락
 *      (뒤쪽 스테이지 확인용. 진행도만 올려서는 선행 검사를 통과하지 못한다)
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
 *  DebugSetStoryProgress 3
 *    → 스토리 진행도를 3으로 강제 설정 (RequiredStoryProgress 게이팅 테스트용)
 *
 * ※ Exec 함수는 에디터/개발 빌드에서만 동작하며 릴리즈 빌드에서는 무시됨
 *===========================================================*/
#pragma region QuestDebug
public:
	// [디버그] 선행 사슬을 완료시키고 건너뛰기 — 콘솔 입력: DebugSkipToQuest Q_Smith_003
	UFUNCTION(Exec)
	void DebugSkipToQuest(FName QuestID);

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

	// [디버그] 스토리 진행도 강제 설정 — 콘솔 입력: DebugSetStoryProgress 3
	UFUNCTION(Exec)
	void DebugSetStoryProgress(int32 NewProgress);
#pragma endregion
#pragma region SaveDebug

public:
	UFUNCTION(Exec)
	void DebugSetHealth(float NewHealth);
	UFUNCTION(Exec)
	void DebugSetBattery(float NewBattery);

#pragma endregion
/*===========================================================
 * [디버그 전용] Skill 시스템 콘솔 테스트 함수
 *
 * ※ 사용법: 에디터 실행 중 콘솔(~ 키)을 열고 아래 명령어 입력
 *
 *  DebugDiscoverSkill Guest.Skill.Camera.Flash
 *    → 해당 SkillTag를 Locked에서 InTheory로 강제 전이 (Book 연동 없이 Runtime API만 호출)
 *
 *  DebugAddSkillProgress Guest.Skill.Progress.Camera.FlashUsed 1.0
 *    → 해당 ProgressEventTag를 요구하는 InTheory Skill들의 진행도를 강제로 누적
 *
 *  DebugSkillStatus Guest.Skill.Camera.Flash
 *    → 해당 SkillTag의 현재 State와 ConditionProgress를 로그로 출력
 *
 * ※ Exec 함수는 에디터/개발 빌드에서만 동작하며 릴리즈 빌드에서는 무시됨
 *===========================================================*/
#pragma region SkillDebug
public:
	// [디버그] Skill 강제 Discover — 콘솔 입력: DebugDiscoverSkill Guest.Skill.Camera.Flash
	UFUNCTION(Exec)
	void DebugDiscoverSkill(FGameplayTag SkillTag);

	// [디버그] Skill 진행도 강제 누적 — 콘솔 입력: DebugAddSkillProgress Guest.Skill.Progress.Camera.FlashUsed 1.0
	UFUNCTION(Exec)
	void DebugAddSkillProgress(FGameplayTag ProgressEventTag, float Amount);

	// [디버그] Skill 상태/진행도 로그 출력 — 콘솔 입력: DebugSkillStatus Guest.Skill.Camera.Flash
	UFUNCTION(Exec)
	void DebugSkillStatus(FGameplayTag SkillTag);
#pragma endregion
	
#pragma region Save

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
// Inventory Toggle은 AGuestCharacter::ToggleInventoryAction에서 처리
};
