#pragma once

#include "CoreMinimal.h"

namespace GuestMapPackage
{
	/**
	 * PIE(Play In Editor) 시 패키지 경로 마지막 세그먼트에 붙는 UEDPIE_<인스턴스>_<원본이름> 접두를 제거해
	 * 에디터/패키지 빌드에서 OpenLevel 등에 쓸 수 있는 경로로 만듭니다.
	 * 예: /Game/maps/OtherWorld/UEDPIE_0_L_TavernMain -> /Game/maps/OtherWorld/L_TavernMain
	 */
	FString StripPIEFromPackagePath(const FString& PackagePath);
}
