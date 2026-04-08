// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#pragma once

#include "CoreMinimal.h"

// 분야별 추가
DECLARE_LOG_CATEGORY_EXTERN(LogGCharacter, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGUI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGItem, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGSystem, Log, All);

// 공통 베이스 매크로 (중복 코드 방지)
#define G_BASE_LOG(Category, Verbosity, Format, ...) UE_LOG(Category, Verbosity, TEXT("[%s : %d] %s"), *FString(__FUNCTION__), __LINE__, *FString::Printf(Format, ##__VA_ARGS__))

// 용도별 단축 매크로
// 일반 카테고리
#define G_LOG(Format, ...) G_BASE_LOG(LogGSystem, Log, Format, ##__VA_ARGS__)
#define G_WARN(Format, ...) G_BASE_LOG(LogGSystem, Warning, Format, ##__VA_ARGS__)
#define G_ERR(Format, ...) G_BASE_LOG(LogGSystem, Error, Format, ##__VA_ARGS__)

// 캐릭터 전용 (Characters/ 폴더 내 사용)
#define G_CHAR_LOG(Format, ...) G_BASE_LOG(LogGCharacter, Log, Format, ##__VA_ARGS__)
#define G_CHAR_WARN(Format, ...) G_BASE_LOG(LogGCharacter, Warning, Format, ##__VA_ARGS__)

// UI 전용 (UI/ 폴더 내 사용)
#define G_UI_LOG(Format, ...) G_BASE_LOG(LogGUI, Log, Format, ##__VA_ARGS__)
#define G_UI_ERR(Format, ...) G_BASE_LOG(LogGUI, Error, Format, ##__VA_ARGS__)

// 사용법 예시
// Character 클래스에서
//G_CHAR_LOG(TEXT("캐릭터 이동 상태 변경"));

// UI 위젯 클래스에서
//G_UI_ERR(TEXT("인벤토리 위젯 로드 실패!"));

//G_LOG(TEXT("test"))