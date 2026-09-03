// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Widget/GuestNarrationWidgetBase.h"
#include "Guest/Data/DataAssets/GNarrationDataAsset.h"
#include "Guest/Subsystem/GSpacetimeSubsystem.h"
#include "Guest/Subsystem/GQuestSubsystem.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"
#include "Engine/GameInstance.h"
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
	// 시간 경과 연출 — 위젯이 닫히기 전에 적용해 둔다.
	// 화면이 페이드아웃되는 동안 세계 날짜가 이미 넘어가 있으므로,
	// 나레이션이 걷히면 플레이어는 바뀐 날짜의 세계를 보게 된다.
	if (CurrentNarrationAsset)
	{
		if (const UWorld* World = GetWorld())
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				if (CurrentNarrationAsset->DaysToAdvanceOnFinish > 0)
				{
					if (UGSpacetimeSubsystem* SpacetimeSys = GI->GetSubsystem<UGSpacetimeSubsystem>())
					{
						SpacetimeSys->AdvanceDay(
							CurrentNarrationAsset->DaysToAdvanceOnFinish,
							CurrentNarrationAsset->HourOnFinish);
					}
				}

				// 진행도도 같은 시점에 올린다 — 게이팅된 NPC는 연출이 걷힌 뒤에 나타나야 한다.
				if (CurrentNarrationAsset->StoryProgressOnFinish > 0)
				{
					if (UGQuestSubsystem* QuestSys = GI->GetSubsystem<UGQuestSubsystem>())
					{
						if (CurrentNarrationAsset->StoryProgressOnFinish > QuestSys->GetStoryProgress())
						{
							QuestSys->SetStoryProgress(CurrentNarrationAsset->StoryProgressOnFinish);
						}
					}
				}
			}
		}
	}

	if (Anim_ScreenFadeOut)
	{
		PendingTransition = ENarrationTransition::ScreenFadeOut;
		PlayAnimation(Anim_ScreenFadeOut);
	}
	else
	{
		CloseNarration();
	}
}

void UGuestNarrationWidgetBase::CloseNarration()
{
	// 위젯을 닫기 전에 읽어둔다. 닫은 뒤에는 세션이 정리돼 애셋 참조가 사라진다.
	const bool bShowDemoEnd = CurrentNarrationAsset && CurrentNarrationAsset->bShowDemoEndOnFinish;

	DeactivateWidget();

	if (!bShowDemoEnd) return;

	if (const UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UGuestUISubsystem* UISys = GI->GetSubsystem<UGuestUISubsystem>())
			{
				UISys->PushWidget(
					GuestGameplayTags::TAG_WidgetStack_Frontend,
					GuestGameplayTags::TAG_Widget_DemoEnd);
			}
		}
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
		CloseNarration();
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
