// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "GDigicamPhotoEntryWidget.generated.h"

class UImage;
class UTextBlock;
class UGPhotoEntryObject;

// ListView 아이템 위젯 — WBP에서 이 클래스를 부모로 지정
UCLASS(Abstract)
class GUEST_API UGDigicamPhotoEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	// 선택/해제 시 OnSelected 델리게이트 브로드캐스트
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> IMG_Thumbnail;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Year;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_PlaceName;

private:
	TWeakObjectPtr<UGPhotoEntryObject> CachedEntryObj;
};
