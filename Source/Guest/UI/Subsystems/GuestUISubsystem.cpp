// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/UI/GameplayTags/GuestGameplayTags.h"
#include "Guest/UI/Settings/GuestUISettings.h"
#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

// ─────────────────────────────────────────────────────────
// USubsystem 인터페이스 구현
// ─────────────────────────────────────────────────────────

/**
 * 서브시스템 생성 조건 설정
 * 전용 서버(Dedicated Server)가 아닐 때만 UI 서브시스템을 생성합니다.
 */
bool UGuestUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer))
    {
        return false;
    }

    if (const UGameInstance* GI = Cast<UGameInstance>(Outer))
    {
        // 서버에서는 UI가 필요 없으므로 생성을 건너뜁니다.
        return !GI->IsDedicatedServerInstance();
    }

    return true;
}

void UGuestUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // 초기화 시 필요한 로직이 있다면 여기에 추가합니다.
}

void UGuestUISubsystem::Deinitialize()
{
    // 등록된 스택 참조를 모두 비워 메모리 누수를 방지합니다.
    StackMap.Empty();

    Super::Deinitialize();
}

// ─────────────────────────────────────────────────────────
// Primary Layout (Stack) 관리
// ─────────────────────────────────────────────────────────

/**
 * UI 레이아웃 위젯에서 레이어별 컨테이너(Stack)를 서브시스템에 등록합니다.
 */
void UGuestUISubsystem::RegisterStack(const FGameplayTag& StackTag, UCommonActivatableWidgetContainerBase* Stack)
{
    if (!Stack)
    {
        UE_LOG(LogTemp, Error, TEXT("[GuestUI] RegisterStack 실패: Stack 포인터가 유효하지 않습니다."));
        return;
    }

    StackMap.Add(StackTag, Stack);
    UE_LOG(LogTemp, Log, TEXT("[GuestUI] Stack 등록 완료: %s"), *StackTag.ToString());
}

/**
 * 등록된 태그를 사용하여 해당 스택 컨테이너를 반환합니다.
 */
UCommonActivatableWidgetContainerBase* UGuestUISubsystem::GetWidgetStack(const FGameplayTag& StackTag) const
{
    if (const TObjectPtr<UCommonActivatableWidgetContainerBase>* Found = StackMap.Find(StackTag))
    {
        return Found->Get();
    }

    return nullptr;
}

// ─────────────────────────────────────────────────────────
// 위젯 Push / Pop 로직
// ─────────────────────────────────────────────────────────

/**
 * GameplayTag를 통해 위젯을 찾고, 적절한 스택에 비동기로 로드하여 Push합니다.
 */
void UGuestUISubsystem::PushWidgetByTag(const FGameplayTag& WidgetTag, FOnWidgetPushed OnPushed)
{
    // 1. UI 설정 에셋에서 태그에 연결된 소프트 클래스 참조를 가져옵니다.
    const UGuestUISettings* Settings = GetDefault<UGuestUISettings>();
    if (!Settings)
    {
        UE_LOG(LogTemp, Error, TEXT("[GuestUI] PushWidgetByTag 실패: GuestUISettings를 찾을 수 없습니다."));
        return;
    }

    const TSoftClassPtr<UUserWidget> SoftClass = Settings->FindWidgetClassByTag(WidgetTag);
    if (SoftClass.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("[GuestUI] PushWidgetByTag 실패: 태그 '%s'에 매핑된 위젯 클래스가 없습니다."), *WidgetTag.ToString());
        return;
    }

    // 2. 이 위젯이 위치해야 할 레이어(Stack) 태그를 결정합니다.
    const FGameplayTag StackTag = ResolveStackTag(WidgetTag);
    UCommonActivatableWidgetContainerBase* Stack = GetWidgetStack(StackTag);

    if (!Stack)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GuestUI] PushWidgetByTag 실패: 레이어 스택 '%s'이 등록되지 않았습니다."), *StackTag.ToString());
        return;
    }

    // 3. 비동기 에셋 로딩 시작
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    Streamable.RequestAsyncLoad(
        SoftClass.ToSoftObjectPath(),
        [this, SoftClass, Stack, WidgetTag, OnPushed]()
        {
            // 로드가 완료되었을 때 실행되는 람다 본문
            UClass* WidgetClass = SoftClass.Get();
            if (!WidgetClass)
            {
                UE_LOG(LogTemp, Error, TEXT("[GuestUI] AsyncLoad 완료 후 클래스 유효성 검사 실패."));
                return;
            }

            // Common UI 컨테이너에 위젯 추가 (자동으로 활성화됨)
            UCommonActivatableWidget* NewWidget = Stack->AddWidget<UCommonActivatableWidget>(WidgetClass);

            if (NewWidget)
            {
                // 글로벌 델리게이트 알림
                OnWidgetPushed.Broadcast(NewWidget);
                
                // 개별적으로 넘겨받은 콜백 알림
                if (OnPushed.IsBound())
                {
                    OnPushed.Broadcast(NewWidget);
                }

                UE_LOG(LogTemp, Log, TEXT("[GuestUI] 위젯 Push 성공: %s (Stack: %s)"), *WidgetTag.ToString(), *Stack->GetName());
            }
        }
    );
}

/**
 * 특정 스택의 최상단 위젯을 비활성화하고 제거합니다.
 */
void UGuestUISubsystem::PopTopWidget(const FGameplayTag& StackTag)
{
    if (UCommonActivatableWidgetContainerBase* Stack = GetWidgetStack(StackTag))
    {
        if (UCommonActivatableWidget* ActiveWidget = Stack->GetActiveWidget())
        {
            // Common UI는 DeactivateWidget 호출 시 제거 로직이 실행됩니다.
            ActiveWidget->DeactivateWidget();
            UE_LOG(LogTemp, Log, TEXT("[GuestUI] 위젯 Pop 완료: %s"), *StackTag.ToString());
        }
    }
}

// ─────────────────────────────────────────────────────────
// 특수 목적 UI 함수
// ─────────────────────────────────────────────────────────

/**
 * 확인 창(Modal)을 띄우고 결과를 콜백으로 전달합니다.
 */
void UGuestUISubsystem::ShowConfirmModal(const FGuestConfirmData& ConfirmData, FOnConfirmResult OnResult)
{
    // TODO: ConfirmData를 서브시스템 멤버 변수에 보관하거나, 위젯 생성 직후 주입하는 로직 필요
    // 현재는 단순 Push 로직만 수행합니다.
    PushWidgetByTag(GuestGameplayTags::TAG_Widget_Confirm);
}

/**
 * NPC 대화 위젯을 열고 대화 데이터를 캐싱합니다.
 */
void UGuestUISubsystem::OpenNPCDialogue(const FText& NPCName, const TArray<FText>& DialogueLines)
{
    // 위젯이 로드된 후 읽어갈 수 있도록 서브시스템 멤버 변수에 데이터를 저장합니다.
    CurrentNPCName = NPCName;
    CurrentDialogueLines = DialogueLines;

    PushWidgetByTag(GuestGameplayTags::TAG_Widget_NPCDialogue);
}

// ─────────────────────────────────────────────────────────
// 내부 헬퍼 함수
// ─────────────────────────────────────────────────────────

/**
 * 특정 위젯 태그가 어떤 레이어(Stack)에 할당되어야 하는지 매핑 규칙을 정의합니다.
 */
FGameplayTag UGuestUISubsystem::ResolveStackTag(const FGameplayTag& WidgetTag) const
{
    using namespace GuestGameplayTags;

    // 1. Frontend 레이어 (최상위 시스템 메뉴 등)
    if (WidgetTag == TAG_Widget_PressAnyKey  ||
        WidgetTag == TAG_Widget_MainMenu     ||
        WidgetTag == TAG_Widget_Options      ||
        WidgetTag == TAG_Widget_LoadingScreen)
    {
        return TAG_WidgetStack_Frontend;
    }

    // 2. GameHUD 레이어 (인게임 고정 UI)
    if (WidgetTag == TAG_Widget_GameHUD ||
        WidgetTag == TAG_Widget_InteractionPrompt)
    {
        return TAG_WidgetStack_GameHUD;
    }

    // 3. GameMenu 레이어 (인벤토리, 포즈 메뉴 등 포커스가 필요한 UI)
    if (WidgetTag == TAG_Widget_CameraUI     ||
        WidgetTag == TAG_Widget_PauseMenu    ||
        WidgetTag == TAG_Widget_QuestJournal ||
        WidgetTag == TAG_Widget_Inventory    ||
        WidgetTag == TAG_Widget_TavernManage ||
        WidgetTag == TAG_Widget_NPCDialogue)
    {
        return TAG_WidgetStack_GameMenu;
    }

    // 4. Modal 레이어 (기본값: 최상단 팝업)
    return TAG_WidgetStack_Modal;
}