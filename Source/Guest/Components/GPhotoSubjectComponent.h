// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GPhotoSubjectComponent.generated.h"

/**
 * 사진에 찍힐 수 있는 피사체 하나를 표시한다.
 *
 * 블록아웃 건물은 StaticMeshActor 수십 개로 흩어져 있어서 오너 액터의 바운즈만으로는
 * "그 건물"을 감쌀 수 없다. 그래서 기본은 이 컴포넌트 자신의 트랜스폼 + BoxExtent이며,
 * 액터 하나가 곧 피사체인 경우에만 bUseOwnerBounds를 켠다.
 *
 * BeginPlay에 UGPhotoSubjectRegistrySubsystem으로 자기등록한다 — 셔터를 누를 때마다
 * 월드 전체 액터를 훑지 않기 위해서다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUEST_API UGPhotoSubjectComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UGPhotoSubjectComponent();

	// 이 피사체를 찍었을 때 FPhotoData::SubjectID에 기록될 값.
	// 사진 퀘스트 목표(EQuestObjectiveType::Photo)의 TargetID와 대조된다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photo Subject")
	FName SubjectID;

	// 화면을 이만큼도 채우지 못하면 "찍었다"고 보지 않는다 (1.0 = 화면 전체).
	// 멀리 점처럼 찍힌 건물이 목표를 진행시키면 안 되므로 존재한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photo Subject",
		meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinScreenCoverage = 0.04f;

	// 가림 표본 9개 중 이 비율만큼도 트이지 않으면 "찍었다"고 보지 않는다.
	// 점유율만으로는 언덕 너머로 삐죽 보이는 원경과 탁 트인 데서 마주 본 것이 갈리지 않는다 —
	// 화면에서 차지하는 넓이는 비슷한데 실제로 보이는 정도가 다르기 때문이다.
	// 0이면 이 검사를 하지 않는다.
	//
	// 기본 0.25는 L_Residential_01에서 재서 정했다. 언덕 너머로 삐죽 보이는 아파트가
	// 11~22%, 정상에서 탁 트인 채로 마주 본 아파트가 56%, 골목에서 올려다본 마을이 78%였다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photo Subject",
		meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinVisibleRatio = 0.25f;

	// 같은 프레임에 여러 피사체가 잡혔을 때의 우선순위 가중치.
	// 랜드마크처럼 "이건 찍으면 이거다" 싶은 대상을 살짝 밀어줄 때 쓴다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photo Subject",
		meta = (ClampMin = "0.01"))
	float ScoreWeight = 1.0f;

	// 켜면 오너 액터의 바운즈를 쓴다. 액터 하나가 통째로 피사체인 경우에만 의미가 있다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photo Subject")
	bool bUseOwnerBounds = false;

	// bUseOwnerBounds가 꺼져 있을 때 쓰는 박스 반크기 (컴포넌트 로컬 기준).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photo Subject",
		meta = (EditCondition = "!bUseOwnerBounds"))
	FVector BoxExtent = FVector(500.0f, 500.0f, 500.0f);

	// 판정에 쓸 월드 공간 박스를 돌려준다. 회전은 무시하고 축정렬 박스로 근사한다 —
	// 블록아웃 단계에서 회전까지 따질 만큼의 정밀도가 필요하지 않다.
	void GetWorldBox(FVector& OutCenter, FVector& OutExtent) const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
