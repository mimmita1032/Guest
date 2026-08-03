// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "GPhotoLibrarySubsystem.h"
#include "Guest/Components/CharacterComponents/GInventoryComponent.h"
#include "Guest/Items/Instance/GItemInstance.h"
#include "Guest/Items/Instance/GPhotoItemInstanceData.h"
#include "Guest/Utils/GLog.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	UGInventoryComponent* FindPlayerInventory(const UGameInstance* GameInstance)
	{
		if (!GameInstance) return nullptr;

		const APawn* Player = UGameplayStatics::GetPlayerPawn(GameInstance->GetWorld(), 0);
		return Player ? Player->FindComponentByClass<UGInventoryComponent>() : nullptr;
	}
}

TArray<FPhotoData> UGPhotoLibrarySubsystem::GetPhotos() const
{
	TArray<FPhotoData> Result;

	const UGInventoryComponent* InvComp = FindPlayerInventory(GetGameInstance());
	if (!InvComp) return Result;

	for (const FInventoryItemHandle& Handle : InvComp->GetAllHandles())
	{
		const UGItemInstance* Instance = InvComp->GetItemByHandle(Handle);
		if (!Instance) continue;

		if (const FGPhotoItemInstanceData* PhotoData = Instance->GetInstanceData<FGPhotoItemInstanceData>())
		{
			Result.Add(PhotoData->PhotoData);
		}
	}

	// 인벤토리는 순서를 보장하지 않으므로(TMap) 촬영 시각으로 정렬해 갤러리 순서를 고정한다
	Result.Sort([](const FPhotoData& A, const FPhotoData& B)
	{
		return A.RealWorldTime < B.RealWorldTime;
	});

	return Result;
}

int32 UGPhotoLibrarySubsystem::GetPhotoCount() const
{
	const UGInventoryComponent* InvComp = FindPlayerInventory(GetGameInstance());
	if (!InvComp) return 0;

	int32 Count = 0;
	for (const FInventoryItemHandle& Handle : InvComp->GetAllHandles())
	{
		const UGItemInstance* Instance = InvComp->GetItemByHandle(Handle);
		if (Instance && Instance->GetInstanceData<FGPhotoItemInstanceData>())
		{
			++Count;
		}
	}
	return Count;
}

void UGPhotoLibrarySubsystem::NotifyPhotoTaken(const FPhotoData& Photo)
{
	OnPhotoAdded.Broadcast(Photo);
}

void UGPhotoLibrarySubsystem::RestorePhotoSnapshots(UGInventoryComponent* InventoryComponent)
{
	if (!InventoryComponent) return;

	UGameInstance* GI = GetGameInstance();
	int32 RestoredCount = 0;

	for (const FInventoryItemHandle& Handle : InventoryComponent->GetAllHandles())
	{
		UGItemInstance* Instance = InventoryComponent->GetItemByHandle(Handle);
		if (!Instance) continue;

		FGPhotoItemInstanceData* PhotoInstance = Instance->GetMutableInstanceData<FGPhotoItemInstanceData>();
		if (!PhotoInstance) continue;

		FPhotoData& Photo = PhotoInstance->PhotoData;

		// 메모리에서 그대로 넘어온 경우(레벨 전환) — 텍스처가 살아 있으므로 재디코딩 불필요
		if (Photo.Snapshot) continue;

		if (Photo.CompressedImage.Num() == 0)
		{
			// 이미지가 없어도 메타데이터(연도/장소/날짜)는 살아 있으므로 항목 자체는 유지한다
			G_WARN(TEXT("사진 복원: 이미지 데이터 없음 — 썸네일 없이 유지 (%d년 %s)"),
				Photo.InGameYear, *Photo.PlaceName.ToString());
			continue;
		}

		Photo.Snapshot = FImageUtils::ImportBufferAsTexture2D(Photo.CompressedImage);
		if (!Photo.Snapshot)
		{
			G_WARN(TEXT("사진 복원: 이미지가 손상됨 (%d년 %s)"),
				Photo.InGameYear, *Photo.PlaceName.ToString());
			continue;
		}

		// 촬영 시와 동일하게 Outer를 GameInstance로 — 레벨 전환에도 GC되지 않게 한다
		if (GI)
		{
			Photo.Snapshot->Rename(nullptr, GI, REN_DontCreateRedirectors | REN_ForceNoResetLoaders);
		}
		++RestoredCount;
	}

	if (RestoredCount > 0)
	{
		G_LOG(TEXT("사진 스냅샷 복원: %d장"), RestoredCount);
	}
}
