// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Guest/Data/DataAssets/GSpacetimeTypes.h"
#include "GCameraComponent.generated.h"

class UTextureRenderTarget2D;
class USceneCaptureComponent2D;
class UKismetRenderingLibrary;
class UGItemDefinition;
class UGPhotoSubjectComponent;

// 사진의 SubjectID를 무엇으로 채울지.
UENUM(BlueprintType)
enum class EPhotoSubjectResolution : uint8
{
	// 프레임 판정을 먼저 하고, 잡히는 피사체가 없으면 촬영 좌표의 PhotoSubjectID로 떨어진다.
	// 피사체 컴포넌트를 아직 배치하지 않은 레벨에서도 기존 퀘스트가 그대로 돌아간다.
	FrameThenPlace	UMETA(DisplayName = "프레임 우선, 없으면 장소"),

	// 프레임 판정만 쓴다. 기준 미달이면 사진에 SubjectID가 남지 않는다.
	// 모든 레벨에 피사체 컴포넌트를 배치한 뒤 여기로 옮긴다.
	FrameOnly		UMETA(DisplayName = "프레임만"),

	// 예전 방식 — 촬영 좌표의 PhotoSubjectID를 그대로 쓴다. 문제 추적용 비상구.
	PlaceOnly		UMETA(DisplayName = "장소만")
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUEST_API UGCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGCameraComponent();

	// 촬영 → 인벤토리에 사진 아이템 지급. 인벤토리에 공간이 없으면 촬영이 실패한다.
	// 성공 여부를 반환하므로 UI에서 실패 피드백을 띄울 수 있다.
	UFUNCTION(BlueprintCallable, Category = "Camera")
	bool TakePhoto(const FPhotoData& Metadata);

	UFUNCTION(BlueprintPure, Category = "Camera")
	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

	// Camera 탭에서 구독
	UPROPERTY(BlueprintAssignable, Category = "Camera")
	FOnPhotoTaken OnPhotoTaken;

	void SetupCapture(USceneCaptureComponent2D* InCapture, UTextureRenderTarget2D* InRenderTarget);

	// 뷰파인더(실시간 미리보기) 켜기/끄기.
	// 켜져 있는 동안 씬 전체가 매 프레임 한 번 더 렌더링되므로, 디지캠을 보고 있을 때만 켠다.
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetViewfinderActive(bool bActive);

	UFUNCTION(BlueprintPure, Category = "Camera")
	bool IsViewfinderActive() const;

	// 지금 뷰파인더에 담긴 것 중 "무엇을 찍는 중인가"를 판정해 SubjectID를 돌려준다.
	// 기준을 넘는 피사체가 없으면 NAME_None.
	//
	// 화면 점유 면적으로 재는 이유: 중앙 트레이스 한 발이나 격자 트레이스는 "화면에 얼마나
	// 크게 담겼는가"를 모른다. 멀리 점처럼 찍힌 것과 화면을 꽉 채운 것이 동점이 된다.
	// 앞으로 얹을 사진 평가·등급, "너무 멀리서 찍었네" 류 반응, DSLR 초점 대상 선정이
	// 전부 이 점수 위에 서야 해서 처음부터 면적으로 잰다.
	UFUNCTION(BlueprintCallable, Category = "Camera|Photo")
	FName ResolveSubject() const;

	// 사진에 최종적으로 기록할 SubjectID를 결정한다.
	// PlaceSubjectID는 촬영 좌표의 FSpacetimeData::PhotoSubjectID —
	// 어느 쪽을 쓸지는 SubjectResolution이 정하므로 호출부는 모드를 몰라도 된다.
	UFUNCTION(BlueprintCallable, Category = "Camera|Photo")
	FName ResolveSubjectID(FName PlaceSubjectID) const;

	// ResolveSubject가 쓰는 판정 점수. 디버그·튜닝용으로 열어둔다.
	// 반환값은 0이면 후보에서 탈락한 것이다.
	float ScoreSubject(const UGPhotoSubjectComponent* Subject, float& OutCoverage) const;

protected:
	virtual void BeginPlay() override;

	// 사진의 SubjectID를 무엇으로 채울지. 기본값은 기존 콘텐츠를 깨지 않는 FrameThenPlace다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Photo")
	EPhotoSubjectResolution SubjectResolution = EPhotoSubjectResolution::FrameThenPlace;

	// 가림 판정에 쓸 트레이스 표본 수를 줄이기 위한 스위치.
	// 끄면 프러스텀 + 면적만으로 판정한다(벽 너머 건물도 찍힌 것으로 친다).
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Photo")
	bool bCheckOcclusion = true;

	// 모든 사진이 공유하는 설계도 (에디터에서 DA_Item_Photo 지정)
	// 개체마다 다른 내용은 FGPhotoItemInstanceData로 들어간다
	UPROPERTY(EditDefaultsOnly, Category = "Camera|Photo")
	TObjectPtr<const UGItemDefinition> PhotoItemDefinition;

private:
	UPROPERTY()
	TObjectPtr<USceneCaptureComponent2D> CaptureComponent;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;
};
