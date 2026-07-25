// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GItemInstanceData.generated.h"

/*========================
아이템 개체별 런타임 데이터의 조상.

UGItemDefinition + Fragments가 "설계도"(같은 종류면 항상 동일한 값)라면,
이쪽은 개체 하나하나가 따로 갖는 상태다.
같은 종류라도 개체마다 값이 달라야 하는 것 — 사진의 촬영 정보, 내구도, 각인,
입수 경위 등 — 이 여기에 붙는다.

사용법: 이 구조체를 상속한 USTRUCT를 만들고 UGItemInstance::SetInstanceData로 주입한다.
FInstancedStruct에 담기므로 세이브(FGuestSavedInventoryEntry::InstanceData)에
그대로 직렬화되며, 나중에 파생 구조체에 필드를 추가해도 구버전 세이브는
기본값으로 채워져 로드되므로 마이그레이션이 필요 없다.

의도적으로 virtual 함수를 두지 않았다 — 순수 데이터 컨테이너이며,
소멸자는 UScriptStruct가 실제 타입을 알고 호출하므로 vtable이 필요 없다.
다형적 동작이 실제로 필요해지면 그때 추가한다.
========================*/
USTRUCT(BlueprintType)
struct GUEST_API FGItemInstanceData
{
	GENERATED_BODY()
};
