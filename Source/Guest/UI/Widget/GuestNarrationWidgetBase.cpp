// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GuestNarrationWidgetBase.h"
#include "Guest/Data/DataAssets/GNarrationDataAsset.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "Engine/Texture2D.h"
#include "TimerManager.h"

void UGuestNarrationWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_Advance)
	{
		Btn_Advance->OnClicked().AddUObject(this, &UGuestNarrationWidgetBase::OnAdvanceClicked);
	}

	FWidgetAnimationDynamicEvent FinishedDelegate;
	FinishedDelegate.BindDynamic(this, &UGuestNarrationWidgetBase::HandleAnimFinished);

	// 같은 완료 콜백을 공유 — 어떤 애니메이션이 끝났는지는 PendingTransition으로 구분
	if (Anim_ImageFadeOut)  BindToAnimationFinished(Anim_ImageFadeOut, FinishedDelegate);
	if (Anim_ScreenFadeOut) BindToAnimationFinished(Anim_ScreenFadeOut, FinishedDelegate);
}

void UGuestNarrationWidgetBase::StartNarrationSession(UGNarrationDataAsset* InAsset)
{
	if (!InAsset || InAsset->Beats.IsEmpty())
	{
		DeactivateWidget();
		return;
	}

	CurrentNarrationAsset = InAsset;
	CurrentBeatIndex = 0;
	ShowBeat(CurrentBeatIndex);

	if (Anim_ScreenFadeIn)
	{
		PlayAnimation(Anim_ScreenFadeIn);
	}
}

void UGuestNarrationWidgetBase::ResetNarrationSession()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
	}

	CurrentNarrationAsset = nullptr;
	CurrentBeatIndex = INDEX_NONE;
	PendingTransition = ENarrationTransition::None;
}

void UGuestNarrationWidgetBase::ShowBeat(int32 BeatIndex)
{
	if (!CurrentNarrationAsset || !CurrentNarrationAsset->Beats.IsValidIndex(BeatIndex)) return;

	const FNarrationBeat& Beat = CurrentNarrationAsset->Beats[BeatIndex];

	if (Image_Illustration)
	{
		if (UTexture2D* Texture = Beat.Illustration.LoadSynchronous())
		{
			Image_Illustration->SetBrushFromTexture(Texture);
		}
	}

	if (Text_Narration)
	{
		Text_Narration->SetText(Beat.Text);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
		if (Beat.AutoAdvanceDelay > 0.f)
		{
			World->GetTimerManager().SetTimer(
				AutoAdvanceTimerHandle, this, &UGuestNarrationWidgetBase::AdvanceBeat,
				Beat.AutoAdvanceDelay, false);
		}
	}
}

void UGuestNarrationWidgetBase::AdvanceBeat()
{
	if (!CurrentNarrationAsset) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
	}

	const int32 NextIndex = CurrentBeatIndex + 1;
	if (!CurrentNarrationAsset->Beats.IsValidIndex(NextIndex))
	{
		FinishNarration();
		return;
	}

	CurrentBeatIndex = NextIndex;

	if (Anim_ImageFadeOut)
	{
		PendingTransition = ENarrationTransition::ImageFadeOut;
		PlayAnimation(Anim_ImageFadeOut);
	}
	else
	{
		// 페이드 연출 없이 즉시 컷 전환
		ShowBeat(CurrentBeatIndex);
		if (Anim_ImageFadeIn) PlayAnimation(Anim_ImageFadeIn);
	}
}

void UGuestNarrationWidgetBase::FinishNarration()
{
	if (Anim_ScreenFadeOut)
	{
		PendingTransition = ENarrationTransition::ScreenFadeOut;
		PlayAnimation(Anim_ScreenFadeOut);
	}
	else
	{
		DeactivateWidget();
	}
}

void UGuestNarrationWidgetBase::HandleAnimFinished()
{
	switch (PendingTransition)
	{
	case ENarrationTransition::ImageFadeOut:
		PendingTransition = ENarrationTransition::None;
		ShowBeat(CurrentBeatIndex);
		if (Anim_ImageFadeIn) PlayAnimation(Anim_ImageFadeIn);
		break;

	case ENarrationTransition::ScreenFadeOut:
		PendingTransition = ENarrationTransition::None;
		DeactivateWidget();
		break;

	default:
		break;
	}
}

void UGuestNarrationWidgetBase::OnAdvanceClicked()
{
	// 전환 애니메이션 재생 중에는 무시 — 클릭 연타로 장면을 건너뛰는 것을 방지
	if (PendingTransition != ENarrationTransition::None) return;

	AdvanceBeat();
}
