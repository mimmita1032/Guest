// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GItemPlacementGhost.generated.h"

class UGItemDefinition;
class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UWidgetComponent;
class UUserWidget;

// UGItemPlacementComponent가 배치 모드 중 마우스를 따라다니게 하는 반투명 프리뷰 액터.
// 실제 배치는 확정 시 AGItemPickup을 별도로 스폰해서 처리하고, 이 액터는 미리보기 전용.
UCLASS()
class GUEST_API AGItemPlacementGhost : public AActor
{
	GENERATED_BODY()

public:
	AGItemPlacementGhost();

	// 미리보기할 아이템의 메시를 세팅
	void SetItemDefinition(const UGItemDefinition* ItemDef);

	// 현재 위치가 배치 가능한지에 따라 고스트 색상을 바꿈 (GhostMaterial에 "Tint" 벡터 파라미터가 있어야 반영됨)
	void SetValidState(bool bValid);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	// "[F] 내려놓기" 안내 문구 — 스크린 스페이스 위젯이라 회전/빌보드 처리 불필요
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UWidgetComponent> PromptWidgetComp;

	// 위 컴포넌트에 띄울 위젯 클래스 — 에디터에서 "[F] 내려놓기" 텍스트 하나 있는 간단한 WBP로 지정
	UPROPERTY(EditDefaultsOnly, Category = "Placement")
	TSubclassOf<UUserWidget> PromptWidgetClass;

	// 반투명 미리보기용 머티리얼 — 에디터에서 지정 (M_Ghost 등, "Tint" 벡터 파라미터 권장)
	UPROPERTY(EditDefaultsOnly, Category = "Placement")
	TObjectPtr<UMaterialInterface> GhostMaterial;

private:
	void ApplyGhostMaterial();

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;
};
