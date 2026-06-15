// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"
#include "Guest/UI/Settings/GuestUISettings.h"

#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

// ─────────────────────────────────────────────────────────
// 1. USubsystem 인터페이스
// ─────────────────────────────────────────────────────────

bool UGuestUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer)) return false;

    if (const UGameInstance* GI = Cast<UGameInstance>(Outer))
    {
        return !GI->IsDedicatedServerInstance();
    }
    return true;
}

void UGuestUISubsystem::Deinitialize()
{
    StackMap.Empty();
    InputConfigMap.Empty();
    ActiveStackHistory.Empty();
    CurrentIMC = nullptr;
    Super::Deinitialize();
}

// ─────────────────────────────────────────────────────────
// 2. Stack 관리
// ─────────────────────────────────────────────────────────

void UGuestUISubsystem::RegisterStack(FGameplayTag StackTag, UCommonActivatableWidgetContainerBase* Stack)
{
    if (StackTag.IsValid() && Stack)
    {
        StackMap.Add(StackTag, Stack);
        UE_LOG(LogTemp, Log, TEXT("[GuestUI] Stack 등록: %s"), *StackTag.ToString());
    }
}

// ─────────────────────────────────────────────────────────
// 3. 입력 Config 등록
// ─────────────────────────────────────────────────────────

void UGuestUISubsystem::RegisterInputConfig(const FGameplayTag& StackTag, const FGuestUIInputConfig& Config)
{
    InputConfigMap.Add(StackTag, Config);
    UE_LOG(LogTemp, Log, TEXT("[GuestUI] InputConfig 등록: %s | Mode: %d"),
        *StackTag.ToString(), static_cast<int32>(Config.InputMode));
}

// ─────────────────────────────────────────────────────────
// 4. 위젯 Push / Pop
// ─────────────────────────────────────────────────────────

void UGuestUISubsystem::PushWidget(FGameplayTag StackTag, FGameplayTag WidgetTag)
{
    if (!StackMap.Contains(StackTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("[GuestUI] Stack '%s' 미등록."), *StackTag.ToString());
        return;
    }
    UCommonActivatableWidgetContainerBase* TargetStack = StackMap[StackTag];

    const UGuestUISettings* Settings = GetDefault<UGuestUISettings>();
    TSoftClassPtr<UUserWidget> SoftClass = Settings->FindWidgetClassByTag(WidgetTag);
    if (SoftClass.IsNull()) return;

    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    Streamable.RequestAsyncLoad(
        SoftClass.ToSoftObjectPath(),
        [this, SoftClass, TargetStack, StackTag]()
        {
            UClass* LoadedClass = SoftClass.Get();
            if (!LoadedClass || !TargetStack) return;

            UCommonActivatableWidget* NewWidget =
                TargetStack->AddWidget<UCommonActivatableWidget>(LoadedClass);

            if (NewWidget)
            {
                OnWidgetPushed.Broadcast(NewWidget);
                UE_LOG(LogTemp, Log, TEXT("[GuestUI] Widget Push: %s"), *NewWidget->GetName());

                // [히스토리 갱신]
                ActiveStackHistory.RemoveSingle(StackTag);
                ActiveStackHistory.Push(StackTag);

                // [입력 갱신]
                CurrentStackTag = StackTag;
                ApplyInputConfig(ResolveInputConfig(StackTag));
            }
        }
    );
}

void UGuestUISubsystem::OpenNPCDialogue(const FNPCDialogueData& Data)
{
    if (Data.Lines.IsEmpty()) return;
    PendingDialogueData = Data;
    PushWidget(GuestGameplayTags::TAG_WidgetStack_GameMenu, GuestGameplayTags::TAG_Widget_NPCDialogue);
}

void UGuestUISubsystem::PopWidget(FGameplayTag StackTag)
{
    // 1. [최상단 위젯 비활성화]
    if (StackMap.Contains(StackTag))
    {
        if (UCommonActivatableWidget* ActiveWidget = StackMap[StackTag]->GetActiveWidget())
        {
            ActiveWidget->DeactivateWidget();
        }
    }

    // 2. [히스토리 업데이트] 닫힌 스택 제거
    ActiveStackHistory.RemoveSingle(StackTag);

    // 3. [입력 롤백 대상 탐색]
    FGameplayTag FallbackStackTag; 
    
    if (ActiveStackHistory.Num() > 0)
    {
        FallbackStackTag = ActiveStackHistory.Top();
    }

    UE_LOG(LogTemp, Log, TEXT("[GuestUI] Widget Pop: %s → Fallback: %s"),
        *StackTag.ToString(),
        FallbackStackTag.IsValid() ? *FallbackStackTag.ToString() : TEXT("None (Default GameOnly)"));

    // 4. [입력 복구]
    CurrentStackTag = FallbackStackTag;
    ApplyInputConfig(ResolveInputConfig(FallbackStackTag));
}

// ─────────────────────────────────────────────────────────
// 5. 입력 내부 헬퍼
// ─────────────────────────────────────────────────────────

FGuestUIInputConfig UGuestUISubsystem::ResolveInputConfig(const FGameplayTag& StackTag) const
{
    if (StackTag == GuestGameplayTags::TAG_WidgetStack_Modal)
    {
        FGuestUIInputConfig ModalConfig;
        ModalConfig.InputMode = EGuestInputMode::UIOnly;

        if (const FGuestUIInputConfig* Found = InputConfigMap.Find(StackTag))
        {
            ModalConfig.MappingContext  = Found->MappingContext;
            ModalConfig.MappingPriority = Found->MappingPriority;
        }
        return ModalConfig;
    }

    if (const FGuestUIInputConfig* Found = InputConfigMap.Find(StackTag))
    {
        return *Found;
    }
    // [추가된 안전망] 어느 태그에서 롤백이 실패했는지 명확히 로그로 출력합니다.
    UE_LOG(LogTemp, Warning, TEXT("[GuestUI] 경고: '%s' 스택의 InputConfig를 찾을 수 없습니다. 기본값(GameOnly)으로 폴백되어 마우스가 사라질 수 있습니다."), 
        StackTag.IsValid() ? *StackTag.ToString() : TEXT("유효하지 않은 태그 (히스토리가 비어있음)"));
    
    return FGuestUIInputConfig{};
}

void UGuestUISubsystem::ApplyInputConfig(const FGuestUIInputConfig& Config)
{
    APlayerController* PC = GetLocalPlayerController();
    if (!PC) return;

    ApplyInputMode(Config.InputMode, PC);

    if (IsValid(Config.MappingContext))
    {
        SwapIMC(Config.MappingContext, Config.MappingPriority, PC);
    }
    else if (IsValid(CurrentIMC))
    {
        // 롤백 시 IMC가 없는 상태(GameOnly)로 돌아가야 한다면 기존 IMC 제거
        ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
        if (LocalPlayer)
        {
            if (UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                EnhancedSubsystem->RemoveMappingContext(CurrentIMC);
                CurrentIMC = nullptr;
            }
        }
    }

    CurrentInputMode = Config.InputMode;
}

void UGuestUISubsystem::ApplyInputMode(EGuestInputMode InputMode, APlayerController* PC)
{
    switch (InputMode)
    {
    case EGuestInputMode::GameOnly:
        PC->SetInputMode(FInputModeGameOnly());
        PC->SetShowMouseCursor(false);
        break;

    case EGuestInputMode::UIOnly:
        PC->SetInputMode(FInputModeUIOnly());
        PC->SetShowMouseCursor(true);
        break;

    case EGuestInputMode::GameAndUI:
        {
            FInputModeGameAndUI Mode;
            Mode.SetHideCursorDuringCapture(false);
            PC->SetInputMode(Mode);
            PC->SetShowMouseCursor(true);
        }
        break;
    }
}

void UGuestUISubsystem::SwapIMC(UInputMappingContext* NewIMC, int32 Priority, APlayerController* PC)
{
    ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
    if (!LocalPlayer) return;

    UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem =
        LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!EnhancedSubsystem) return;

    if (IsValid(CurrentIMC))
    {
        EnhancedSubsystem->RemoveMappingContext(CurrentIMC);
    }

    EnhancedSubsystem->AddMappingContext(NewIMC, Priority);
    CurrentIMC = NewIMC;
}

APlayerController* UGuestUISubsystem::GetLocalPlayerController() const
{
    const UGameInstance* GI = GetGameInstance();
    if (!GI) return nullptr;

    return GI->GetFirstLocalPlayerController();
}