#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/UI/Settings/GuestUISettings.h"
#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

bool UGuestUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer)) return false;
    // 전용 서버가 아닐 때만 생성
    if (const UGameInstance* GI = Cast<UGameInstance>(Outer))
    {
        return !GI->IsDedicatedServerInstance();
    }
    return true;
}

void UGuestUISubsystem::Deinitialize()
{
    StackMap.Empty();
    Super::Deinitialize();
}

void UGuestUISubsystem::RegisterStack(FGameplayTag StackTag, UCommonActivatableWidgetContainerBase* Stack)
{
    if (StackTag.IsValid() && Stack)
    {
        StackMap.Add(StackTag, Stack);
        UE_LOG(LogTemp, Log, TEXT("Stack Registered: %s"), *StackTag.ToString());
    }
}

void UGuestUISubsystem::PushWidget(FGameplayTag StackTag, FGameplayTag WidgetTag)
{
    // 1. 목표 스택 검사
    if (!StackMap.Contains(StackTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("Stack Tag %s not found!"), *StackTag.ToString());
        return;
    }
    UCommonActivatableWidgetContainerBase* TargetStack = StackMap[StackTag];

    // 2. 위젯 클래스 설정 로드
    const UGuestUISettings* Settings = GetDefault<UGuestUISettings>();
    TSoftClassPtr<UUserWidget> SoftClass = Settings->FindWidgetClassByTag(WidgetTag);

    if (SoftClass.IsNull()) return;

    // 3. 비동기 로딩 ( 스타일 핵심)
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    Streamable.RequestAsyncLoad(
        SoftClass.ToSoftObjectPath(),
        [this, SoftClass, TargetStack]()
        {
            UClass* LoadedClass = SoftClass.Get();
            if (!LoadedClass || !TargetStack) return;

            // CommonUI의 AddWidget으로 스택에 추가
            UCommonActivatableWidget* NewWidget = TargetStack->AddWidget<UCommonActivatableWidget>(LoadedClass);
            
            if (NewWidget)
            {
                OnWidgetPushed.Broadcast(NewWidget);
                UE_LOG(LogTemp, Log, TEXT(" Pushed Widget: %s"), *NewWidget->GetName());
            }
        }
    );
}

void UGuestUISubsystem::PopWidget(FGameplayTag StackTag)
{
    if (StackMap.Contains(StackTag))
    {
        if (UCommonActivatableWidget* ActiveWidget = StackMap[StackTag]->GetActiveWidget())
        {
            ActiveWidget->DeactivateWidget();
        }
    }
}