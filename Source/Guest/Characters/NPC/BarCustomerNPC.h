// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Guest/Interfaces/GInteractableInterface.h"
#include "BarCustomerNPC.generated.h"

class UCameraComponent;
class UGDialogueDataAsset;

/**
 * ABarCustomerNPC
 *
 * 바 카운터 좌석에 앉아 대화하는 손님 NPC.
 * 바 매니저(미구현)가 StartBarDialogue()를 직접 호출해 대화를 시작한다.
 * 테스트용으로 임시 E키 상호작용(Interact_Implementation)을 구현해 놓음.
 *
 * DialogueCamera: 바 카운터 너머에서 NPC 상반신을 바라보는 시점.
 * 대화 시작 시 플레이어 카메라가 이 카메라로 블렌드된다.
 */
UCLASS()
class GUEST_API ABarCustomerNPC : public ACharacter, public IGInteractableInterface
{
	GENERATED_BODY()

public:
	ABarCustomerNPC();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractText_Implementation() const override;

	/** 바 매니저 또는 테스트 코드에서 직접 호출해 대화를 시작한다. */
	UFUNCTION(BlueprintCallable, Category = "Bar|Dialogue")
	void StartBarDialogue();

	/** GBarDialogueWidget::NativeOnDeactivated에서 호출 - 카메라를 플레이어로 복귀. */
	UFUNCTION(BlueprintCallable, Category = "Bar|Dialogue")
	void EndBarDialogue();

	UCameraComponent* GetDialogueCamera() const { return DialogueCamera; }

protected:
	/** 카메라 복귀 블렌드 완료 후 플레이어 이동 입력을 복구한다. */
	UFUNCTION()
	void RestorePlayerInput();

private:
	FTimerHandle CameraRestoreTimerHandle;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> CachedPC;

public:
	/** NPC 상반신을 바라보는 대화 전용 카메라. 에디터에서 위치/회전 조정. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bar|Camera")
	TObjectPtr<UCameraComponent> DialogueCamera;

	/** 이 NPC의 대화 데이터. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bar|Dialogue")
	TObjectPtr<UGDialogueDataAsset> DialogueAsset;

	/** 대화 종료 시 퀘스트 목표(Talk) 달성으로 브로드캐스트할 ID. 비어있으면 전달하지 않음. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bar|Quest")
	FName TalkObjectiveID;

	/** 카메라 전환 블렌드 시간 (초). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bar|Camera")
	float CameraBlendTime = 0.5f;
};
