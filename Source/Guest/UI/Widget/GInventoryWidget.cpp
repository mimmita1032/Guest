// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GInventoryWidget.h"
#include "GInventorySlotWidget.h"
#include "GInventoryItemWidget.h"
#include "GEquipSlotWidget.h"
#include "GQuickSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Components/CharacterComponents/GItemPlacementComponent.h"
#include "Guest/Items/Definition/GItemDefinition.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "Guest/Items/Fragments/GItemFragmentInventory.h"
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

	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (UGItemPlacementComponent* PlacementComp = OwningPawn->FindComponentByClass<UGItemPlacementComponent>())
		{
			PlacementComp->OnPlacementStateChanged.AddDynamic(this, &UGInventoryWidget::HandlePlacementStateChanged);
		}
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

	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (UGItemPlacementComponent* PlacementComp = OwningPawn->FindComponentByClass<UGItemPlacementComponent>())
		{
			PlacementComp->OnPlacementStateChanged.RemoveDynamic(this, &UGInventoryWidget::HandlePlacementStateChanged);

			if (PlacementComp->IsPlacementActive())
			{
				PlacementComp->CancelPlacement();
			}
		}
	}

	if (UGuestUISubsystem* UISys = GetUISubsystem())
	{
		UISys->NotifyWidgetDeactivated(GuestGameplayTags::TAG_WidgetStack_GameMenu);
	}
}

FReply UGInventoryWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (!bPlacementModeActive) return Reply;

	APawn* OwningPawn = GetOwningPlayerPawn();
	UGItemPlacementComponent* PlacementComp = OwningPawn ? OwningPawn->FindComponentByClass<UGItemPlacementComponent>() : nullptr;
	if (!PlacementComp) return Reply;

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		PlacementComp->ConfirmPlacement();
		return Reply.Handled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		PlacementComp->CancelPlacement();
		return Reply.Handled();
	}

	return Reply;
}

void UGInventoryWidget::RequestPlaceItem(FInventoryItemHandle Handle)
{
	if (!InventoryComponent) return;

	const UGItemInstance* Instance = InventoryComponent->GetItemByHandle(Handle);
	const UGItemDefinition* ItemDef = Instance ? Instance->GetItemDef() : nullptr;
	if (!ItemDef) return;

	APawn* OwningPawn = GetOwningPlayerPawn();
	UGItemPlacementComponent* PlacementComp = OwningPawn ? OwningPawn->FindComponentByClass<UGItemPlacementComponent>() : nullptr;
	if (!PlacementComp) return;

	PlacementComp->BeginPlacement(ItemDef, Handle);
}

void UGInventoryWidget::HandleItemRightClicked(FInventoryItemHandle Handle, FVector2D ScreenPosition)
{
	if (bPlacementModeActive) return;
	BP_ShowItemContextMenu(Handle, ScreenPosition);
}

void UGInventoryWidget::HandlePlacementStateChanged(bool bActive)
{
	bPlacementModeActive = bActive;
	SetRenderOpacity(bActive ? 0.3f : 1.0f);
	SetVisibility(bActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Visible);

	for (const TObjectPtr<UGInventoryItemWidget>& ItemWidget : SpawnedItemWidgets)
	{
		if (ItemWidget)
		{
			ItemWidget->SetInteractionLocked(bActive);
		}
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

	// 1. 기존 위젯 비우기
	Grid_Inventory->ClearChildren();
	Canvas_Items->ClearChildren();
	SpawnedItemWidgets.Reset();

	if (EquipSlot_Helmet) EquipSlot_Helmet->RefreshSlotUI(nullptr);
	if (EquipSlot_Chest)  EquipSlot_Chest->RefreshSlotUI(nullptr);
	if (EquipSlot_Legs)   EquipSlot_Legs->RefreshSlotUI(nullptr);
	if (EquipSlot_Boots)  EquipSlot_Boots->RefreshSlotUI(nullptr);

	// 2. 그리드 배경 슬롯 생성
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
			NewSlot->OnSlotItemDropped.AddDynamic(this, &UGInventoryWidget::HandleSlotItemDropped);

			if (UUniformGridSlot* GridSlot = Grid_Inventory->AddChildToUniformGrid(NewSlot, Row, Col))
			{
				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}

	// [헬퍼 람다] 아이템 위젯 생성
	// RuntimeIcon은 사진처럼 개체마다 그림이 다른 아이템용. 없으면 Icon이 쓰인다
	auto CreateItemWidget = [&](FInventoryItemHandle Handle, FIntPoint Size, TSoftObjectPtr<UTexture2D> Icon,
								UTexture2D* RuntimeIcon = nullptr) -> UGInventoryItemWidget*
	{
		UGInventoryItemWidget* ItemWidget = CreateWidget<UGInventoryItemWidget>(GetOwningPlayer(), ItemWidgetClass);
		if (ItemWidget)
		{
			ItemWidget->InitItem(Handle, Icon, Size, SlotSize, RuntimeIcon);
			ItemWidget->OnItemDroppedOutside.AddDynamic(this, &UGInventoryWidget::HandleItemDroppedOutside);
			ItemWidget->OnItemRightClicked.AddDynamic(this, &UGInventoryWidget::HandleItemRightClicked);
			ItemWidget->SetInteractionLocked(bPlacementModeActive);
			SpawnedItemWidgets.Add(ItemWidget);
		}
		return ItemWidget;
	};

	// 3. 아이템 렌더링 (그리드 및 장비창)
	const TArray<FInventoryItemHandle> Handles = InventoryComponent->GetAllHandles();

	for (const FInventoryItemHandle& Handle : Handles)
	{
		FInventoryItemRenderData RenderData = InventoryComponent->GetItemRenderData(Handle);

		// A. 그리드에 있는 아이템
		if (RenderData.bIsValid && RenderData.Position.X >= 0 && RenderData.Position.Y >= 0)
		{
			if (UGInventoryItemWidget* ItemWidget = CreateItemWidget(Handle, RenderData.GridSize, RenderData.Icon, RenderData.RuntimeIcon))
			{
				if (UCanvasPanelSlot* CanvasSlot = Canvas_Items->AddChildToCanvas(ItemWidget))
				{
					CanvasSlot->SetAutoSize(false);
					CanvasSlot->SetPosition(FVector2D(RenderData.Position.X * SlotSize, RenderData.Position.Y * SlotSize));
					CanvasSlot->SetSize(FVector2D(RenderData.GridSize.X * SlotSize, RenderData.GridSize.Y * SlotSize));
				}
			}
			continue;
		}

		// B. 장비창에 있는 아이템
		UGItemInstance* ItemInst = InventoryComponent->GetItemByHandle(Handle);
		if (!ItemInst) continue;

		TSoftObjectPtr<UTexture2D> EquipIcon;
		if (const UGItemFragmentInventory* InvFrag = ItemInst->FindFragmentByClass<UGItemFragmentInventory>())
		{
			EquipIcon = InvFrag->ItemIcon;
		}

		// 장비 슬롯 크기에 맞게 1x1 사이즈로 생성
		UGInventoryItemWidget* EquipItemWidget = CreateItemWidget(Handle, FIntPoint(1, 1), EquipIcon, RenderData.RuntimeIcon);

		if (InventoryComponent->GetEquippedItem(EEquipSlot::Helmet) == Handle && EquipSlot_Helmet)
			EquipSlot_Helmet->RefreshSlotUI(EquipItemWidget);
		else if (InventoryComponent->GetEquippedItem(EEquipSlot::Chest) == Handle && EquipSlot_Chest)
			EquipSlot_Chest->RefreshSlotUI(EquipItemWidget);
		else if (InventoryComponent->GetEquippedItem(EEquipSlot::Legs) == Handle && EquipSlot_Legs)
			EquipSlot_Legs->RefreshSlotUI(EquipItemWidget);
		else if (InventoryComponent->GetEquippedItem(EEquipSlot::Boots) == Handle && EquipSlot_Boots)
			EquipSlot_Boots->RefreshSlotUI(EquipItemWidget);
	}

	// 4. 퀵슬롯 렌더링
	TArray<UGQuickSlotWidget*> QuickSlotWidgets = { QuickSlot_0, QuickSlot_1, QuickSlot_2, QuickSlot_3 };
	for (int32 i = 0; i < 4; ++i)
	{
		if (UGQuickSlotWidget* QWidget = QuickSlotWidgets[i])
		{
			FInventoryItemHandle QHandle = InventoryComponent->GetQuickSlotItem(i);
			
			if (QHandle.IsValid())
			{
				TSoftObjectPtr<UTexture2D> QIcon = nullptr;
				if (UGItemInstance* QInst = InventoryComponent->GetItemByHandle(QHandle))
				{
					if (const UGItemFragmentInventory* InvFrag = QInst->FindFragmentByClass<UGItemFragmentInventory>())
					{
						QIcon = InvFrag->ItemIcon;
					}
				}

				// ★ 핵심 변경점: 퀵슬롯에도 장비창처럼 1x1 사이즈의 위젯을 직접 생성해서 부착합니다.
				UGInventoryItemWidget* QuickItemWidget = CreateItemWidget(QHandle, FIntPoint(1, 1), QIcon,
					InventoryComponent->GetItemRenderData(QHandle).RuntimeIcon);
				QWidget->RefreshSlotUI(QuickItemWidget);
			}
			else
			{
				// 핸들이 유효하지 않은(비어있는) 경우
				QWidget->RefreshSlotUI(nullptr);
			}
		}
	}

	G_LOG(TEXT("인벤토리 UI 갱신 완료 (장비 및 퀵슬롯 연동): 총 %d칸, 아이템 %d개 배치"), TotalSlots, Handles.Num());
}

void UGInventoryWidget::HandleItemDroppedOutside(FInventoryItemHandle Handle)
{
	if (!Handle.IsValid()) return;
	if (!InventoryComponent) return;

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