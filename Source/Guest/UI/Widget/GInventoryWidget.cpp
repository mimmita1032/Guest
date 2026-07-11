// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventoryWidget.h"
#include "GInventorySlotWidget.h"
#include "GInventoryItemWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/UI/Subsystems/GuestUISubsystem.h"
#include "Guest/GameplayTags/GuestGameplayTags.h"
#include "Guest/Utils/GLog.h"
#include "Guest/Sound/GuestSoundSubsystem.h"
#include "Guest/Sound/GuestSoundTags.h"

void UGInventoryWidget::SetInventoryComponent(UGInventoryComponent* InComponent)
{
	if (InComponent)
	{
		InventoryComponent = InComponent;
		G_LOG(TEXT("인벤토리 위젯: 컴포넌트 연결 완료"));
	}
}

void UGInventoryWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (!InventoryComponent)
	{
		if (APawn* OwningPawn = GetOwningPlayerPawn())
		{
			if (UGInventoryComponent* InvComp = OwningPawn->FindComponentByClass<UGInventoryComponent>())
			{
				SetInventoryComponent(InvComp);
			}
		}
	}

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &UGInventoryWidget::OnRefreshInventory);
	}

	OnRefreshInventory();
}

void UGInventoryWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UGInventoryWidget::OnRefreshInventory);
	}

	if (UGuestUISubsystem* UISys = GetUISubsystem())
	{
		UISys->NotifyWidgetDeactivated(GuestGameplayTags::TAG_WidgetStack_GameMenu);
	}
}

void UGInventoryWidget::NativeDestruct()
{
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveAll(this);
	}
	Super::NativeDestruct();
}

TOptional<FUIInputConfig> UGInventoryWidget::GetDesiredInputConfig() const
{
	// GuestUISubsystem이 결정한 값을 그대로 반환 — 여기서 값을 따로 하드코딩하면
	// CommonUI ActionRouter와 GuestUISubsystem이 서로 다른 입력모드를 주장할 수 있음
	if (const UGuestUISubsystem* UISys = GetUISubsystem())
	{
		return UISys->GetDesiredUIInputConfig();
	}
	return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::NoCapture);
}

void UGInventoryWidget::OnRefreshInventory()
{
	if (!InventoryComponent || !Grid_Inventory || !SlotWidgetClass || !Canvas_Items || !ItemWidgetClass)
	{
		G_WARN(TEXT("인벤토리 갱신 실패: 패널이나 클래스 설정이 누락되었습니다."));
		return;
	}

	Grid_Inventory->ClearChildren();
	Canvas_Items->ClearChildren();

	const int32 Columns   = InventoryComponent->Columns;
	const int32 Rows      = InventoryComponent->Rows;
	const int32 TotalSlots = Columns * Rows;

	for (int32 i = 0; i < TotalSlots; ++i)
	{
		if (UGInventorySlotWidget* NewSlot = CreateWidget<UGInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass))
		{
			const int32 Row = i / Columns;
			const int32 Col = i % Columns;

			NewSlot->SetSlotPosition(Col, Row);

			// OnRefreshInventory 호출마다 위젯을 새로 생성하므로 중복 바인딩 없음
			NewSlot->OnSlotItemDropped.AddDynamic(this, &UGInventoryWidget::HandleSlotItemDropped);

			if (UUniformGridSlot* GridSlot = Grid_Inventory->AddChildToUniformGrid(NewSlot, Row, Col))
			{
				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}

	const TArray<FInventoryItemHandle> Handles = InventoryComponent->GetAllHandles();

	for (const FInventoryItemHandle& Handle : Handles)
	{
		const FInventoryItemRenderData RenderData = InventoryComponent->GetItemRenderData(Handle);
		if (!RenderData.bIsValid) continue;
		if (RenderData.Position.X < 0 || RenderData.Position.Y < 0) continue;

		if (UGInventoryItemWidget* ItemWidget = CreateWidget<UGInventoryItemWidget>(GetOwningPlayer(), ItemWidgetClass))
		{
			ItemWidget->InitItem(Handle, RenderData.Icon, RenderData.GridSize, SlotSize);
			ItemWidget->OnItemDroppedOutside.AddDynamic(this, &UGInventoryWidget::HandleItemDroppedOutside);

			if (UCanvasPanelSlot* CanvasSlot = Canvas_Items->AddChildToCanvas(ItemWidget))
			{
				CanvasSlot->SetAutoSize(false);
				CanvasSlot->SetPosition(FVector2D(RenderData.Position.X * SlotSize, RenderData.Position.Y * SlotSize));
				CanvasSlot->SetSize(FVector2D(RenderData.GridSize.X * SlotSize, RenderData.GridSize.Y * SlotSize));
			}
		}
	}

	G_LOG(TEXT("인벤토리 갱신 완료: 총 %d칸, 아이템 %d개 배치"), TotalSlots, Handles.Num());
}

void UGInventoryWidget::HandleItemDroppedOutside(FInventoryItemHandle Handle)
{
	if (!Handle.IsValid()) return;
	if (!InventoryComponent) return;

	//사운드: 인벤토리 밖으로 버릴 때 (드롭 사운드)
	if (UGuestSoundSubsystem* SoundSys = GetGameInstance()->GetSubsystem<UGuestSoundSubsystem>())
	{
		SoundSys->PlayGlobalSound(GuestSoundTags::TAG_Sound_Event_UI_ButtonClick, AudioDataAsset);
	}

	InventoryComponent->DropItem(Handle);
}

void UGInventoryWidget::HandleSlotItemDropped(FInventoryItemHandle Handle, int32 TargetX, int32 TargetY)
{
	if (!Handle.IsValid()) return;
	if (!InventoryComponent) return;
	InventoryComponent->MoveItem(Handle, TargetX, TargetY);
}