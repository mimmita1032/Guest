# 게임 진행 흐름 (Game Progression)

이 문서는 게임 오프닝부터 실제 플레이 순서대로 어떤 일이 벌어지는지, 그리고 각 시점이 `StoryProgress`(`GQuestSubsystem`)의 몇 단계에 해당하는지를 정리한다.
목적: 만들어둔 시스템(퀘스트/대화/인벤토리/세이브 등)을 실제 플레이 가능한 한 줄기 경로로 연결하고, 아직 없는 기능을 이 문서 기준으로 파악한다.

## 사용법
- 확정된 진행 단계만 표에 채운다. 미정인 부분은 `TBD`로 남긴다.
- `StoryProgress` 값은 `FQuestData::RequiredStoryProgress` / `StoryProgressOnComplete`와 그대로 대응해야 한다.
- 구현 여부(구현됨 / 부분구현 / 미구현)를 표시해서, 이 문서만 보고 다음에 뭘 만들어야 하는지 알 수 있게 한다.

### 노션 문서와의 역할 분담

내용이 겹치면 반드시 한쪽이 낡는다. 이렇게 나눈다.

| | 담는 것 |
|---|---|
| **이 문서** | 데모 진행 흐름, 스테이지별 구현 현황, 설계 결정과 그 이유. 코드와 함께 버전 관리된다 |
| **노션 「퀘스트 작성 가이드」** | 퀘스트를 새로 만들 때의 작업 절차, 필드별 의미, 함정, 검증 방법 |
| **노션 「퀘스트 DB」** | 퀘스트별 실제 기입값 (Steps, Target ID, 대화 노드 등) |

**"어떤 필드에 뭘 넣는가"는 이 문서에 쓰지 않는다.** 노션을 가리킨다.

## 데모 범위

**데모 종료 지점: 스미스 퀘스트 체인 완결 (`Q_Smith_003` 완료 + 엔딩 재등장)**

데모 한 판으로 보여주려는 것:
1. 세계관 — 여기(주점)와 저기(현실)의 대비, 시공간 이동 능력
2. 핵심 루프 — 의뢰 수주 → 시공간 이동 → 수거/촬영 → 귀가 → 전달 → 보상
3. NPC 서사 — 스미스 한 명의 이야기가 시작부터 끝까지 닫힘

### 퀘스트 ID 네이밍
**코드(`DT_QuestData`) 기준인 `Q_Smith_001` 형식으로 통일한다.**
노션 「퀘스트 흐름 개요」의 `Q_S_SMT_001` 표기는 이 문서 기준으로 맞춰 갱신할 것.

---

## 진행 단계표

| StoryProgress | 위치/씬 | 사건 | 플레이어 행동 | 다음 단계 트리거 | 구현 상태 |
|---|---|---|---|---|---|
| 0 | 현실 · 들판 (`L_Field_01`) | 오프닝. 들판에 핀 꽃을 발견 | 꽃을 화분에 옮겨 담는다 | 화분 들고 주점으로 이동 | 구현됨 |
| 1 | 주점 (`L_TavernMain`) | 귀가. 주점에 화분을 놓는다 | 테이블에 화분 배치 | 배치 완료 → 나레이션 재생 | 구현됨 |
| 2 | 주점 (`L_TavernMain`) | 나레이션 종료 후 **하루 경과**. 스미스 첫 방문 | 맥주 접객 → 의뢰 수주 | 대화 완료 → `Q_Smith_001` 수락 | 구현됨 |
| 3 | 현실 · 주택가 (`L_Residential_01`) | 평범한 건물 촬영 | 주택가 풍경 촬영 | 사진 확보 후 귀가 | 부분구현 |
| 4 | 주점 (`L_TavernMain`) | 사진 전달. 스미스가 "영감이 안 온다"며 보수 지불 | 스미스와 대화 | `Q_Smith_001` 완료 | 구현됨 |
| 5 | 주점 (`L_TavernMain`) | 스미스 재방문. 독특한 건물 의뢰 | 대화 | `Q_Smith_002` 수락 | 미구현 |
| 6 | 현실 · 랜드마크 3종 | 독특한 건물 촬영 | 박물관/전통건축/스카이스파이어 촬영 | 사진 전달 → 스미스가 스카이스파이어 선택 | 미구현 |
| 7 | 현실 · 스카이스파이어 (`L_Landmark_SkySpire_01`) | 추가 의뢰 — 입체 모형 수거 | 내부 전망대에서 모형 획득 | 모형 들고 귀가 | 미구현 |
| 8 | 주점 (`L_TavernMain`) | 모형 전달. 스미스 보수 지불 후 한동안 이탈 | 스미스와 대화 | `Q_Smith_003` 완료 | 미구현 |
| 9 | 중앙 도시 (`L_CitySquare`) | **엔딩** — 스미스가 완성된 홀로그램 설계도를 보여주며 감사 인사 | 관람 | 데모 종료 화면 | 미구현 |

---

## ⚠️ 선행 시스템 작업

콘텐츠 배치보다 **먼저** 끝내야 하는 것들. 나중에 하면 이미 만든 퀘스트/세이브를 다시 손봐야 한다.

> **진행 상황 (2026-07-26): A·B·C·D 코드 작업 완료.** 브랜치 `feat/item-instance-data`.
> **(2026-08-05): 에디터 작업 1~5 완료 — Stage 0~1이 한 줄기로 완주된다.** 브랜치 `feat/quest-content-04`.
> 남은 선행 작업은 **E(데모 엔딩 화면)** 뿐이고, 다음은 Stage 2(스미스 첫 방문) 콘텐츠다.

### A. 인벤토리 개체별 데이터 (per-instance data) — ✅ 완료

현재 구조는 **"같은 종류면 완전히 동일한 아이템"** 을 전제한다.

```cpp
// 현재 — 개체 고유 데이터를 담을 곳이 없다
class UGItemInstance {
    TObjectPtr<const UGItemDefinition> ItemDef;  // 설계도 포인터
    FInventoryItemHandle Handle;                 // 식별자
};

// 세이브도 ItemID 문자열 기반 — 같은 ID면 전부 같은 것으로 복원된다
struct FGuestSavedInventoryEntry {
    FName ItemID;
    FIntPoint TopLeft, Size;
    int32 Quantity;
};
```

사진은 한 장 한 장이 다르므로 이 전제를 깬다. `UGItemDefinition`의 `Fragments`(설계도 조립)에 대응하는 **런타임 짝**이 비어 있는 상태이며, 이는 사진만의 문제가 아니다 — 내구도, 각인, 수집 시각, 입수 경위 등 앞으로 필요한 모든 개체별 상태가 같은 빈칸을 쓴다.

- ✅ `FGItemInstanceData` — 개체별 데이터의 조상. `UGItemInstance`가 `FInstancedStruct`로 보유
- ✅ `SetInstanceData`는 계보를 검사해 거부하고, `GetInstanceData<T>`는 `static_assert`로 잘못된 타입을 컴파일 단계에서 차단
- ✅ `GrantItemWithData` / `PlaceItemAt` 경로 추가 (기존 `GrantItem`은 위임만 하므로 호출부 불변)
- ✅ `FGuestSavedInventoryEntry::InstanceData` 세이브 왕복
- ✅ 구버전 세이브는 빈 값으로 로드되어 그대로 동작 — 마이그레이션 불필요

### B. 사진의 아이템화 — ✅ 완료

**목표: 촬영하면 사진이 인벤토리 아이템으로 들어가고, 디지캠 「찍은 사진」 메뉴에서도 확인된다.**

현재는 사진과 아이템이 완전히 분리된 두 세계다.

| | 현재 사진 | 현재 아이템 |
|---|---|---|
| 저장소 | `GPhotoLibrarySubsystem`의 `TArray<FPhotoData>` | `GInventoryComponent` 그리드 |
| 정체 | 구조체(값) | `UGItemInstance`(오브젝트) |
| 세이브 | `SavedPhotos` 별도 배열 | `SavedInventory` |
| UI | 디지캠 갤러리 탭 | 인벤토리 그리드 |

- ✅ `FGPhotoItemInstanceData` — 사진 개체 데이터
- ✅ 촬영 시 인벤토리에 사진 아이템 지급. 공간이 없으면 픽셀 읽기·PNG 압축 **전에** 취소하고 `TakePhoto`가 `false` 반환 (디지캠 이동 자체는 막지 않음)
- ✅ `GPhotoLibrarySubsystem`을 자체 배열에서 **인벤토리 조회 뷰**로 전환
- ✅ `SavedPhotos` 별도 배열 제거 (인벤토리 세이브로 일원화)
- ✅ `RestorePhotoSnapshots` — 세이브 로드 시 PNG에서 텍스처 복원. 레벨 전환처럼 텍스처가 살아 있으면 재디코딩을 건너뛴다
- ✅ 인벤토리 복원과 사진 복원을 `GuestGameInstance::RestoreInventory`로 묶음 (호출부 3곳 중복 제거)
- ⬜ **(에디터)** `DA_Item_Photo` 생성 + 카메라 컴포넌트에 지정 — 아래 「에디터에서 해야 할 일」 참고

`FPhotoData`에는 이미 촬영 장소·시간이 들어 있다 — `InGameYear` / `AreaCode` / `PlaceName` / `StoryDate` / 촬영 시각. 별도 추가 불필요.

> **DSLR 기능은 최종적으로 전부 들어간다** — 노출 보정, 조리개·셔터·ISO, 초점(피사계심도), 뷰파인더 등.
> 다만 데모에서는 조작 UI를 만들지 않고, **구조만 미리 열어둔다.**
>
> 구체적으로 이 작업(B)에서 지켜야 할 것:
> - 사진 개체 데이터는 **촬영 설정(FCameraSettings 같은 것)을 나중에 필드로 붙일 수 있는 형태**로 만든다.
>   지금은 비어 있어도 되지만, 나중에 필드를 추가할 때 세이브 마이그레이션이 필요 없어야 한다.
> - 촬영 경로(`GCameraComponent` → 사진 생성)를 **한 군데로 모아둔다.**
>   나중에 노출·초점이 결과 이미지에 영향을 주려면 이 지점 하나만 고치면 되도록.
>
> 이렇게 해두면 DSLR 기능 추가가 "기존 구조에 파라미터를 얹는 일"이 되고, 인벤토리·세이브를 다시 건드릴 필요가 없다.

### C. Photo 퀘스트 목표 타입 — ✅ 완료

- ✅ `EQuestObjectiveType::Photo` 추가
- ✅ `FPhotoData::SubjectID` — 무엇을 찍었는가. 퀘스트 목표의 `TargetID`와 대조
- ✅ `FSpacetimeData::PhotoSubjectID` — 그 좌표에서 찍은 사진에 기록될 촬영 대상 식별자
- ✅ 촬영 시 `SubjectID`가 지정돼 있으면 `OnObjectiveUpdated` 브로드캐스트

**판정 방식 (확정):** **장소 단위**로 동작한다 — 한 좌표 = 한 피사체.
사진의 `SubjectID`는 **촬영 당시 플레이어가 있던 좌표**의 `PhotoSubjectID`에서 온다.

`SubjectID`라는 한 겹을 둔 덕분에, 추후 카메라에 트레이스가 생기면 **그 필드를 실제 피사체로 채우기만 하면 되고** `DT_QuestData`의 목표 데이터와 판정 로직은 그대로 둘 수 있다.

> 따라서 아파트/주택가/달동네는 현재 구조에서 **각각 다른 좌표(`DT_SpacetimeData` 행)** 로 두어야 구분된다.
> 피사체 조준(트레이스)이 생기면 한 레벨 안에 세 피사체를 두는 방식으로 바꿀 수 있다.

### C-2. 촬영과 이동의 분리 — ✅ 완료

**바뀐 것**: 원래 디지캠 셔터 하나가 "이동 직전 자동 촬영 + 시공간 이동"을 겸했다.
그래서 **찍히는 그림은 출발지인데 사진에 박히는 정보는 목적지**였다 —
들판에서 주택가 좌표를 입력하고 셔터를 누르면 들판 사진에 "주택가 · `Photo_House`"가 붙어,
스미스의 주택가 사진 의뢰가 들판 사진으로 완료됐다.

- ✅ 이동에 딸린 자동 촬영 제거
- ✅ `UGDigicamComponent::HandleShutter()` — **촬영 전담**. 지금 있는 곳을 찍는다
- ✅ `UGDigicamComponent::HandleTravel()` — **이동 전담**. 좌표가 맞춰졌을 때만 동작
- ✅ `IA_DigicamTravel` — 이동 확정용 별도 입력. 되돌릴 수 없는 행동이라 셔터와 키를 나눴다
- ✅ `UGSpacetimeSubsystem::GetCurrentLocation()` — **현재 좌표**. 사진의 장소·연도·`SubjectID`의 단일 출처
  - `DoTravel()`에서 도착지를 확정
  - `PostLoadMapWithWorld`에서 레벨 이름으로 역조회 (문 이동·세이브 로드처럼 `ExecuteTravel`을 거치지 않은 경로)
  - `EnsureCurrentLocation()` — 좌표가 비어 있으면 촬영 시점에 다시 확정한다
  - 세이브 연동 (`SavedLocationYear` / `SavedLocationAreaCode` — 좌표가 아니라 좌표를 찾는 열쇠만 저장)

> ⚠️ **`PostLoadMapWithWorld`는 PIE 시작 레벨에 대해 오지 않는다.** 실측으로 확인했다 —
> 맵이 GameInstance보다 먼저 준비되는 경로라 첫 레벨만 훅을 놓친다.
> 그래서 `EnsureCurrentLocation()`이 없으면 **시작 레벨에서 찍은 사진에만 장소가 비었다.**
> 앞으로 현재 좌표를 읽는 코드를 새로 쓸 때는 델리게이트가 이미 돌았다고 가정하지 말 것.

> ⚠️ 촬영이 필요한 레벨은 **반드시 `DT_SpacetimeData`에 행이 있어야 한다.**
> 행이 없으면 그 레벨의 사진에는 장소 정보가 비어 들어가고 로그에 경고가 남는다.

**남은 이름 빚**: `FOnShutterDenied` 델리게이트는 실제로는 *이동* 거부 시에만 발생한다.
셔터가 이동을 겸하던 시절의 이름인데, 기존 BP 바인딩을 깨지 않으려 그대로 뒀다.

### C-3. 뷰파인더 비용 — ✅ 완료

`SetupCapture()`가 BeginPlay에서 `bCaptureEveryFrame = true`로 켠 뒤 한 번도 끄지 않았다.
디지캠을 꺼내지 않은 평상시에도 **씬 전체가 매 프레임 두 번 렌더링**되고 있었다.

- ✅ `SetupCapture()` 기본값을 `false`로
- ✅ `UGCameraComponent::SetViewfinderActive(bool)` 추가 — 켜는 순간 한 장 그려 첫 프레임 빈 화면 방지
- ✅ `ActivateDigicam()` / `DeactivateDigicam()`에서 켜고 끈다

> 디지캠을 **닫을 때는 `OnTabDeactivated`가 오지 않는다** (`SwitchTab`에서 탭 전환 시에만 호출).
> 그래서 끄는 책임은 `DeactivateDigicam()`에 둬야 한다.
>
> 더 좁히고 싶다면 `WBP_DigiTab_Camera`에서 `OnTabActivated` / `OnTabDeactivated`에
> `SetViewfinderActive`를 물려 **Camera 탭을 보고 있을 때만** 돌게 할 수 있다 (선택).
> 그 경우에도 위의 `DeactivateDigicam()` 처리는 남겨둘 것.

### D. 날짜 시스템 — ✅ 완료

- ✅ `GSpacetimeSubsystem::CurrentDay` (1일차부터)
- ✅ 자정 경과 시 자동 진행. 히치로 하루를 여러 번 넘겨도 넘긴 만큼 정확히 센다
- ✅ `AdvanceDay(NumDays, NewHour)` — 시각을 함께 지정할 수 있어 "다음 날 아침 9시"를 한 번에 표현
- ✅ `OnDayChanged` 델리게이트
- ✅ 세이브 연동 (`SavedWorldDay`, 구버전은 -1 센티널로 1일차 유지)

**날짜 진행 트리거 (확정):** 자정 경과(자동) + 나레이션 종료(데이터 지정).
`UGNarrationDataAsset::DaysToAdvanceOnFinish` / `HourOnFinish`를 채우면 나레이션이 끝날 때 날짜가 넘어간다.
화면이 페이드아웃되는 동안 적용되므로, 나레이션이 걷히면 이미 다음 날이다.

### 진행도도 나레이션이 올린다 — `StoryProgressOnFinish`

스토리 진행도를 올리는 곳은 **둘**이고, 어느 쪽을 쓰느냐가 곧 시점을 정한다.

| 올리는 곳 | 시점 |
|---|---|
| `FQuestData::StoryProgressOnComplete` | 퀘스트가 완료되는 즉시 |
| `UGNarrationDataAsset::StoryProgressOnFinish` | 나레이션이 끝나고 화면이 걷힐 때 |

처음에는 퀘스트 쪽만 있었는데, 그러면 **연출 도중에 세계가 바뀐다.** `CompleteQuest`가
진행도를 올리면 그 안에서 곧바로 `OnQuestListChanged`가 브로드캐스트되고, 그것을 구독하는
NPC가 가시성을 재평가한다. 나레이션 요청은 그보다 뒤라서, **스미스가 나레이션과 동시에
나타나버렸다** — 하룻밤이 지나고 찾아와야 할 손님이 하루가 지나기도 전에 앉아 있었다.

그래서 진행도 상승을 나레이션 종료 시점으로 옮겼다. 날짜도 진행도도 같은 자리에서 바뀌므로
**세계가 바뀌는 시점이 한 곳으로 모인다.**

→ Stage 1의 "꽃 배치 → 나레이션 → 하루 뒤 스미스"가 **코드 수정 없이 데이터만으로** 완성된다.
`Q_Field_002`는 진행도를 올리지 않고, `DA_Narration_Tavern_001`이 `StoryProgressOnFinish = 2`로 올린다.

> 나레이션이 붙은 퀘스트에서 **둘을 동시에 쓰지 않는다.** 둘 다 채우면 진행도가 두 번 올라
> 게이팅이 엉뚱하게 풀린다.

### E. 데모 엔딩 화면

- ⬜ 데모 종료 화면 (`Credit|Ending|Demo` 관련 코드 현재 전무)
- 나레이션 시스템(`UGNarrationDataAsset` + `WBP_Narration`)을 그대로 재사용하면 신규 위젯 없이 처리 가능

---

## 상세 — Stage 0: 오프닝 (현실 · 들판)

- **씬**: 현실 세계의 들판. 주인공이 핀 꽃 한 송이를 화분에 옮겨 심는다.
- **레벨**: `L_Field_01` (`Content/maps/Earth/Field/L_Field_01.umap`)
- **의도**: 튜토리얼 겸 세계관 제시 — 시간과 공간을 이동할 수 있다는 것을 첫 이동으로 보여준다.
- **플레이어 행동**: 퀘스트 UI가 뜨고, 들판을 돌아다니다 특정 꽃과 상호작용 → 화분에 담긴 꽃이 인벤토리에 들어옴 → 트래커가 다음 단계(귀가)로 넘어감
- **다음 단계 트리거**: 화분 들고 주점으로 이동 → 씬 전환
  - 이동 수단: 디지캠(`UGDigicamComponent`) → `GSpacetimeSubsystem::ExecuteTravel` → `OpenLevel`
- **구현 현황**:
  - ✅ `AGItemPickup` — 꽃 줍기·인벤토리 획득·퀘스트 목표 갱신
  - ✅ `AGQuestAutoStarter` — 레벨 진입 시 NPC 없이 퀘스트 자동 수락
  - ✅ `DA_Item_Flower` (`Content/Items/Definitions/Quest/`)
  - ✅ `DT_QuestData`의 `Q_Field_001` '분위기 전환' (Step01: Collect(Flower))
  - ✅ `L_Field_01`에 `AGItemPickup`(꽃) + `AGQuestAutoStarter` 배치
  - ✅ **(에디터)** `Q_Field_001`을 꽃 줍기까지로 축소 + `StoryProgressOnComplete = 1` (아래 「퀘스트 분할」)
- **구현 상태**: 구현됨 — 실측 확인 완료 (꽃 줍기 → 진행도 1 → `Q_Field_002` 자동 수락)

### 퀘스트 분할 — 주점 이동 게이팅 (확정)

**문제**: `Tavern` 좌표의 `RequiredStoryProgress`가 `0`이라 **꽃을 줍지 않고도 주점에 갈 수 있다.**
퀘스트 흐름상 주점 이동 조건은 "꽃을 인벤토리에 넣는 것"인데 그게 강제되지 않는다.

**구조적 제약**: `StoryProgressOnComplete`는 **퀘스트 단위** 필드다(`FQuestData`). 단계 단위가 아니다.
그런데 원래 설계는 꽃 줍기와 주점 도착·배치가 **같은 퀘스트의 다른 단계**였다.
그러면 진행도가 오르는 시점이 주점에서 화분을 놓은 *뒤*라서, 주점 이동을 막는 데 쓸 수 없다.

**채택안 — 퀘스트를 둘로 쪼갠다** (데이터만으로 해결). ✅ 적용 완료:

| 퀘스트 | 범위 | 핵심 필드 |
|---|---|---|
| `Q_Field_001` '분위기 전환' | 꽃 줍기까지 | `StoryProgressOnComplete = 1`, `NextQuestID = Q_Field_002` |
| `Q_Field_002` '꽃의 자리' | 귀가(Step02) + 화분 배치(Step03) | `RequiredStoryProgress = 1`, `NarrationOnComplete = DA_Narration_Tavern_001` |

그리고 `DT_SpacetimeData`의 `Tavern` 행에 **`RequiredStoryProgress = 1`**. ✅
→ 꽃을 줍기 전에는 주점 좌표가 `Locked`으로 뜨고 이동이 거부된다.

> ⚠️ **미검증**: "꽃을 줍지 않고 주점 이동을 시도하면 거부된다"는 경로는 아직 실제로 밟아보지 않았다.
> 데이터는 넣었으나 거부 동작 자체는 확인 필요.

**주의 — `NextQuestID`는 비어 있어도 경고가 없다.** `GQuestSubsystem.cpp:194`가
`if (!NextQuestID.IsNone())`로 조용히 건너뛰므로, 연결이 안 되면 로그에 아무것도 남지 않는다.
"퀘스트 완료" 다음 줄에 "연결 퀘스트 … 자동 수락"이 없으면 이 필드를 의심할 것.

**기각안**: `FQuestStepData`에 단계 단위 `StoryProgressOnComplete`를 추가하는 방법.
퀘스트를 하나로 유지할 수 있지만 개념이 하나 늘어난다. 스미스 체인도 퀘스트 단위로 끊기므로
퀘스트 분할 쪽이 일관된다. 다만 단계 단위 진행도 자체는 나중에 쓸 데가 있어 TODO에 남긴다.

## 상세 — Stage 1: 귀가 (주점 Guest)

> ⚠️ **정정**: 이전 판에서 이 단계를 "현실세계 · 집"으로 적었으나 틀렸다.
> 여기서 말하는 "집"은 **이세계 주점 `L_TavernMain`** 이다. 라무스에게 주점이 곧 집이다.

- **씬**: 디지캠으로 주점 좌표로 이동 → 도착 시 "귀가" 목표 자동 완료 → 테이블에 화분을 올려두면 Step 완료
- **레벨**: `L_TavernMain` (`Content/maps/Beyond/Tavern/L_TavernMain.umap`) — 노션 레벨 DB상 "에셋 배치" 30%
- **플레이어 행동**: 주점 도착 → 테이블(`AGItemPlacementPoint`)과 상호작용 → 인벤토리의 화분이 제거되고 테이블 위에 시각적으로 배치됨
- **다음 단계 트리거**: 배치 완료 → **나레이션 재생** → 하루 경과 → 스미스 등장
- **구현 현황**:
  - ✅ `AGQuestReachTrigger` — Reach 목표 자동 완료
  - ✅ `AGItemPlacementPoint` — 인벤토리에서 제거 후 월드에 시각화 + Place 목표 브로드캐스트
  - ✅ `EQuestObjectiveType::Place`
  - ✅ 나레이션 시스템 (`UGNarrationDataAsset`, `WBP_Narration`, `GQuestSubsystem::OnNarrationRequested`)
  - ✅ (에디터) `DT_QuestData`에 **`Q_Field_002`** 신설 (Step02 Reach → Step03 Place) — 「퀘스트 분할」 참고
  - ✅ (에디터) `L_TavernMain`에 `AGQuestReachTrigger`(`Reach_Home`) + `AGItemPlacementPoint`(RequiredItem=DA_Item_Flower) 배치
  - ✅ (에디터) `DT_SpacetimeData`의 `Tavern` 행 (2026 / 0)
  - ✅ (에디터) `Tavern` 행의 `RequiredStoryProgress`를 `1`로 — 꽃을 줍기 전에는 주점에 못 가게
  - ⬜ (에디터) 나레이션 애셋 본문 작성 — 현재 `DA_Narration_Tavern_001`은 테스트용 텍스트 상태
  - ✅ 날짜 시스템(선행작업 D) 연동 — `DaysToAdvanceOnFinish = 1`, `HourOnFinish = 9.0`
- **구현 상태**: 구현됨 — 실측 확인 완료
  (도착 → Step02 완료 → Step03 → 화분 배치 → `Q_Field_002` 완료 → 나레이션 → `1일 경과 → 2일차 9.00시`)

## 상세 — Stage 2~4: 스미스 첫 방문 (`Q_Smith_001` 평범한 것들의 기록)

- **씬**: 주점. 스미스가 첫 손님으로 들어온다.
- **대화 흐름** (노션 「스미스」 문서 기준):
  1. 호기롭게 들어와 맥주 한 잔 주문 → 원샷 (호탕한 성격 제시)
  2. 두 번째 맥주를 주문하며 자신이 건축가임을 밝힘
  3. 주인공이 주점을 물려받았다는 것을 언급, 마음에 드는 건물이 없었다고 한탄
  4. 현실 세계의 건물들을 보면 영감이 떠오를지도 모르겠다며 의뢰
- **수거 대상**: **주택가 사진 1장** (`Photo_House`). 좌표는 `DT_SpacetimeData`의 `Residential_2010` 행 → `L_Residential_01`, 2010/7
- **결말**: 스미스는 "이미 비슷한 걸 지어봤다"며 영감을 못 얻지만, 다음엔 독특한 것을 달라고 하며 **보수 지불**

> **사진 3종 → 1장 축소 (확정).** 판정이 장소 단위(한 좌표 = 한 피사체)라 3종을 구분하려면
> 좌표 세 개가 필요한데, 셋이 같은 레벨을 가리키면 플레이어는 같은 풍경을 세 번 찍게 되고
> 레벨을 셋으로 나누면 작업량이 3배가 된다. 데모의 핵심 루프 시연에는 1장으로 충분하다.
> 늘릴 때는 `DT_SpacetimeData`에 좌표 행을 더하고 Step01에 목표만 추가하면 된다.

### 퀘스트 구조

수락은 **대화**가, 완료는 **Talk 목표**가 맡는다.

```
Q_Smith_001  '평범한 것들의 기록'   (RequiredStoryProgress = 2)
├ Step01  Photo  Photo_House    "주택가의 평범한 풍경을 찍어오자"
└ Step02  Talk   Smith_Deliver  "스미스에게 사진을 보여주자"

수락: DT_Smith의 Smith_Order_05 노드에 QuestEventID = Q_Smith_001
```

**`TalkObjectiveID`가 NPC당 하나뿐**이기 때문이다. 스미스는 「의뢰 수주」와 「사진 전달」 두 번
대화하는데 Talk 목표 ID는 하나만 가질 수 있어서, 수락을 Talk 목표로 두면 전달에 쓸 것이 없다.

목표 갱신이 현재 단계만 훑으므로, 대화가 끝날 때마다 `Smith_Deliver`가 브로드캐스트돼도
Step01(Photo) 중에는 무시된다. 그래서 이 구조가 안전하다.

**선택지 조건**(`FDialogueChoice::ConditionID`)은 이때 구현했다. 필드만 있고 아무도 읽지 않아
선택지가 늘 전부 보였다 — 첫 만남부터 "(사진을 보여준다)"가 떠 있었다. 이제 `Q_Smith_001`은
"그 퀘스트 진행 중", `Q_Smith_001.Step02`는 "진행 중이고 현재 단계가 Step02"로 해석된다.
단계까지 볼 수 있어야 "사진을 찍어온 뒤에만" 같은 조건이 표현된다. 사용법은 노션 가이드 참고.

- **구현 현황**:
  - ✅ NPC 등장 게이팅 — `RequiredStoryProgress` / `ApplyStoryProgressVisibility()`
  - ✅ 바 대화 시스템 — `ABarCustomerNPC`, `WBP_BarDialogue`
  - ✅ 선행작업 A·B·C (사진 아이템화 + Photo 목표)
  - ✅ (에디터) `DT_QuestData`의 `Q_Smith_001` 행
  - ✅ (에디터) 스미스 대화 노드 (`DT_Smith` 9행 + `DA_Smith_Dialogue` 연결)
  - ✅ (에디터) `BP_Smith` 등장 설정 및 `L_TavernMain` 배치
  - ✅ 판정 기준 확정 — 장소 단위 (선행작업 C)
  - ⬜ (에디터) `L_Residential_01` 촬영 구역 제작 — 실제로 찍을 만한 풍경이 필요하다
- **구현 상태**: 부분구현 — 퀘스트·대화 흐름은 완성. 촬영지 레벨만 남음

## 상세 — Stage 5~6: 스미스 재방문 (`Q_Smith_002` 독특한 건물 탐색)

- **활성화 조건**: `Q_Smith_001` 완료 후 일정 경과 (노션 기준). 데모에서는 `StoryProgress` 게이팅으로 처리
- **대화**: 다시 맥주 두 잔. 삶이 무료해지고 있으며 정말 놀라운 것을 만들어 보고 싶다고 토로
- **수거 대상**: 독특한 건물 사진 — 후보로 기존 랜드마크 맵 활용
  - `L_Landmark_Museum_01`
  - `L_Landmark_Traditional_01`
  - `L_Landmark_SkySpire_01`
- **결말**: 스미스가 그중 **스카이스파이어**를 선택 → "좀 더 입체적으로 보고 싶다"며 추가 의뢰
- **구현 현황**:
  - ⬜ 전부 미구현. 선행작업 A·B·C 의존
  - ⬜ (에디터) 랜드마크 3개 맵 촬영 대상 정비
  - ⬜ 사진 여러 장 중 하나를 NPC가 고르는 대화 분기 — 기존 대화 시스템으로 가능한지 검증 필요
- **구현 상태**: 미구현

## 상세 — Stage 7~8: 모형 수거 (`Q_Smith_003` 전망대 모형 수거)

- **씬**: 스카이스파이어 내부 전망대
- **레벨**: `L_Landmark_SkySpire_01` — 노션 레벨 DB "현실 세계 — 스카이스파이어 내부" (미시작). **`ITEM_SKY_001` 배치 필요**
- **플레이어 행동**: 전망대에서 스카이스파이어 모형 획득 → 귀가 → 스미스에게 전달
- **결말**: 스미스가 감사를 표하고 보수 지불 후 한동안 주점에 나타나지 않음
- **구현 현황**:
  - ✅ `AGItemPickup` (기존 시스템 재사용)
  - ⬜ (에디터) `ITEM_SKY_001` 아이템 정의 생성
  - ⬜ (에디터) `L_Landmark_SkySpire_01` 내부 전망대 제작 + 모형 배치
- **구현 상태**: 미구현

## 상세 — Stage 9: 엔딩 (중앙 도시)

- **씬**: 중앙 도시 광장. 스미스가 스카이스파이어를 닮은 건축물의 **홀로그램 설계도**를 보여주며 감사 인사
- **레벨**: `L_CitySquare` (`Content/maps/Beyond/City/L_CitySquare.umap`) — 노션 레벨 DB "중앙 도시 광장" (미시작)
- **결말**: 데모 종료 화면
- **구현 현황**:
  - ⬜ 선행작업 E (데모 엔딩 화면)
  - ⬜ (에디터) `L_CitySquare` 제작 + 홀로그램 연출
  - ⬜ (에디터) 엔딩 나레이션 애셋
- **구현 상태**: 미구현

---

## 에디터에서 해야 할 일

코드로는 만들 수 없어 에디터에서 직접 해야 하는 것들. **위쪽이 더 급하다.**

### 1. 사진 아이템 설계도 — ✅ 완료
- ✅ `DA_Item_Photo` (`Content/Items/Definitions/`) — `ItemID = Photo`,
  `UGItemFragmentInventory`(`GridSize = 2×2`, `ItemIcon` 비움 — 사진은 찍힌 장면이 아이콘이 된다)
- ✅ 플레이어 블루프린트의 **Camera 컴포넌트 → `Photo Item Definition`** 에 위 애셋 지정

> **촬영 파이프라인 실측 확인 완료** — 들판에서 SNAP → `사진 촬영 완료: 2026년 들판` → 인벤토리 진입(2×2).

### 1-2. 촬영/이동 분리에 따른 에디터 작업 — 이것도 없으면 이동이 안 된다
- ✅ **`IA_DigicamTravel`** 인풋 액션 애셋 생성 → IMC 바인딩 → `BP_GuestCharacter`에 지정
- ✅ `WBP_DigiTab_Collection`의 `HandleShutter` 호출 노드를 `HandleTravel`로 교체
- ✅ `WBP_DigiTab_Camera`의 `SNAP` 버튼을 `HandleShutter`에 연결 (`On Clicked` → Cast → Get Digicam Component → Handle Shutter)
- ✅ `DT_SpacetimeData`에 `Field_2026` 행 (`L_Field_01`, 2026/1, `RequiredStoryProgress = 999` — 한 번 떠나면 못 돌아감)

### 2. 촬영 대상 지정 (사진 퀘스트용)
- ✅ `Residential_2010` 행 (`L_Residential_01`, 2010/7, `PhotoSubjectID = Photo_House`)
- ✅ `Tavern` 행 (`L_TavernMain`, 2026/0, `PhotoSubjectID` 비움 — 의도된 것)
- ~~아파트/달동네 — 레벨을 나눌지 피사체 조준을 도입할지 미정~~
  → **데모에서는 주택가 1장으로 축소 (확정).** 근거는 「상세 — Stage 2~4」 참고.
  늘릴 때는 `DT_SpacetimeData`에 좌표 행을 더하고 Step01에 목표만 추가하면 된다

### 3. 시간 경과 연출 — ✅ 완료
- ✅ `DA_Narration_Tavern_001`의 **`Days To Advance On Finish`** = `1`
- ✅ **`Hour On Finish`** = `9.0` (다음 날 아침)

> **함정**: `Hour On Finish`에는 `EditCondition = "DaysToAdvanceOnFinish > 0"`이 걸려 있다
> (`GNarrationDataAsset.h:49`). `Days`를 1로 **확정하기 전에** `Hour`를 타이핑하면 값이 들어가지 않는다.
> 기본값 `-1.0`은 "시각은 건드리지 않음"이라, 이 경우 날짜만 넘어가고 시계는 흐르던 그대로다.
> 로그에 `→ 2일차 9.00시`가 아니라 어중간한 시각이 찍히면 이걸 의심할 것.

### 4. 퀘스트 분할 (주점 이동 게이팅 — 「Stage 0 · 퀘스트 분할」 참고) — ✅ 완료
- ✅ `DT_QuestData`의 `Q_Field_001`을 **꽃 줍기까지로 축소**
  - `StoryProgressOnComplete = 1`, `NextQuestID = Q_Field_002`, `NarrationOnComplete` 비움
- ✅ `DT_QuestData`에 **`Q_Field_002`** 신설 (Step02 Reach → Step03 Place)
  - `RequiredStoryProgress = 1`, `NarrationOnComplete = DA_Narration_Tavern_001`
- ✅ `DT_SpacetimeData`의 `Tavern` 행 → **`RequiredStoryProgress = 1`**

### 5. 나머지
- ⬜ `WBP_Narration`에 `Anim_ScreenFadeIn` / `Anim_ScreenFadeOut` (선택 — 없으면 컷 전환)
- ✅ `L_TavernMain`에 `AGQuestReachTrigger` + `AGItemPlacementPoint` 배치

> ⚠️ **기존 세이브의 사진은 유실된다.** `SavedPhotos` 배열을 제거하고 인벤토리로 일원화했기 때문이다.
> 개발 단계라 마이그레이션 코드는 넣지 않았다. 새로 시작하면 문제없다.

## 작업 순서 권고

1. ~~**선행 시스템** — A(개체별 데이터) → B(사진 아이템화) → C(Photo 목표) → D(날짜)~~ ✅ 코드 완료
2. ~~**에디터 작업 1~3** — 특히 `DA_Item_Photo`가 없으면 촬영 자체가 동작하지 않는다~~ ✅ 완료
3. ~~**Stage 1 마무리** — 주점 배치 + 나레이션까지 한 줄기로 플레이 가능하게~~ ✅ 완료
4. **Stage 2~4** — 스미스 첫 방문. 여기서 핵심 루프가 처음으로 완주된다 ← **지금 여기**
5. **Stage 5~8** — 스미스 체인 나머지
6. **Stage 9 + E** — 엔딩과 데모 종료 화면

## 데모 이후 예정
- **DSLR 촬영 기능 본편** — 노출 보정, 조리개·셔터스피드·ISO, 초점/피사계심도, 뷰파인더 UI
  (선행작업 B에서 구조를 열어두므로 인벤토리·세이브 재작업 없이 얹을 수 있어야 한다)
- **인화 모델 (사진 저장 구조 개편)** — 아래 참조
- 메인 퀘스트 체인 (노션상 미기획)
- 히든 퀘스트 (노션상 미기획)

### 인화 모델 — 사진 저장 구조 개편 (데모 이후)

**문제**: 시공간 이동 시 자동 촬영이라 이동할 때마다 사진이 인벤토리 칸을 먹는다.

**채택안**: 찍은 것은 전부 디지캠(갤러리)에 남고, 플레이어가 갤러리에서 골라 **"인화"** 했을 때
비로소 인벤토리 아이템이 된다. 분기를 *퀘스트 유무*가 아니라 **플레이어 선택**으로 옮기는 것.
서사적으로도 맞다 — 디지캠에 있는 건 데이터고, 남에게 건네려면 인화해야 한다.

**기각안**: "퀘스트가 있으면 인벤토리, 없으면 디지캠에 저장" — 채택하지 않는다.
- 촬영이 자동이라 플레이어가 통제할 수 없는 분기다. 퀘스트 수락 **전에** 찍은 사진은
  영영 퀘스트에 못 쓰고, 플레이어는 "찍었는데 왜 없어?"를 겪는다
- 저장소가 둘로 갈리면 세이브·복원·갤러리·스냅샷 텍스처 복원이 전부 두 갈래가 된다.
  `SavedPhotos`를 지우고 인벤토리로 일원화한 이유가 정확히 이것이었다
- 디지캠에만 있는 사진은 NPC에게 건넬 수단이 없다

**데모 범위에서는**: 저장소는 인벤토리 하나로 유지한다. 칸 압박은 `GridSize`를 작게 두고
필요 없는 사진을 버릴 수 있게 하는 선에서 해결한다.

**구현 시 주의**: 갤러리 저장소를 다시 만드는 작업이므로, 지금의
"저장소는 인벤토리 하나 / 갤러리(`UGPhotoLibrarySubsystem`)는 그걸 읽는 뷰" 구조가 바뀐다.
인화된 사진과 인화되지 않은 사진의 세이브 경로를 처음부터 하나로 설계할 것.

## TODO

### ✅ 완료 — Stage 1 완주 (2026-08-05)
꽃 줍기부터 화분 배치 후 하루 경과까지 실측 확인 완료. 아래 셋 모두 끝났다.

1. ~~`DA_Narration` 시간 경과 설정~~ → `DA_Narration_Tavern_001`, `Days = 1` / `Hour = 9.0`
2. ~~퀘스트 분할~~ → `Q_Field_001` 축소 + `Q_Field_002` 신설 + `Tavern` 행 게이팅
3. ~~`L_TavernMain` 배치~~ → 기존 배치분으로 충족 (`Reach_Home` 트리거 + 배치 지점)

### ✅ 완료 — Stage 2 스미스 첫 방문 (2026-08-07)

`Q_Smith_001` 행, `DT_Smith` 대화 노드 9행, `BP_Smith` 배치·설정까지 끝났다.
스미스에게 말을 걸어 의뢰를 받는 데까지 실측 확인 완료.

> **`Q_Field_002`의 `NextQuestID`는 비워둔다 (확정).** 예전 계획은 여기에 `Q_Smith_001`을
> 걸어 자동 수락시키는 것이었으나, **스미스 대화가 퀘스트를 수락**시키는 구조로 바꿨다.
> 하루가 지나면 스미스가 등장하고, `DialogueTrigger`에 플레이어가 들어가면 대화가
> 자동으로 시작되므로 별도 안내 없이 이어진다. 비어 있는 것이 정상이니 채우지 말 것.

### ✅ 완료 — 갤러리 썸네일 회색 (2026-08-10)

디지캠 갤러리 목록의 썸네일이 항상 회색으로만 보이던 문제. 원인은 코드가 아니라
`WBP_DigiTab_PhotoEntry`의 **`Border` → `Content Color and Opacity`가 `(0,0,0,0)`** 이었던 것.

이 값은 `Border` **자식 전부에 곱해진다.** 0이면 자식이 그리는 모든 색이 죽는다.
`IMG_Thumbnail`만 들여다봐서는 절대 안 나온다 — 브러시·틴트·가시성·크기 전부 정상으로
보이고, 실제로 정상이었다. 글자가 멀쩡했던 건 원래 검정이라 곱해도 티가 안 나서고,
상세 패널(`IMG_SelectedPhoto`) 사진이 정상이었던 건 그 `Border` 밖이라서다.

> **교훈: 자식 위젯이 "설정은 다 맞는데 안 보인다"면 부모의 `Content Color and Opacity`와
> `Is Enabled`를 먼저 볼 것.** 리플렉터의 자식 속성만 봐서는 원인이 드러나지 않는다.

### 다음 할 일

1. **`L_Residential_01` 촬영 구역 제작** ← **지금 하는 것** (`feat/residential-blockout`)
   Stage 2~4를 완주시키려면 실제로 찍을 풍경이 필요하다.
   현재 좌표(`Residential_2010`)와 `PhotoSubjectID`(`Photo_House`)는 이미 있다.
   **구성 (확정): 고저차 있는 달동네. 꼭대기까지 올라가면 아파트 단지가 원경으로 보인다.**
   낡은 저층과 새 고층의 대비가 스미스의 "평범한 건물" 의뢰를 그림으로 먼저 말해주고,
   미련/공존 엔딩의 주제와도 맞는다. 아래 2번이 들어오면 두 피사체를 한 레벨에서 나눌 수 있다
2. **피사체 인식 (프레임 기반)** — 위 블록아웃 직후. 아래 「피사체 인식 설계」 참고
3. **사진 비율·해상도 정리 (고도화 때)** — 촬영본이 `1920x1080`인데 썸네일 브러시는
   `300x180`이라 상세 패널에서 사진이 찌그러진다. 또 한 장이 약 8MB(BGRA)라
   장수가 쌓이면 메모리와 세이브 용량이 그대로 곱해진다. 촬영 해상도를 낮추거나
   썸네일용 축소본을 따로 만드는 쪽으로 정리할 것
4. **UI 텍스트 넘침 정리** — 전체화면에서 목표 문구·대사·버튼 라벨이 화면 밖으로 잘린다.
   UMG TextBlock에 줄바꿈 기준 폭이 없어서다. 브랜치 `feat/packaging-test`에서 진행 중
5. **패키징 테스트 환경** — `GameDefaultMap`·`bCookAll`은 잡았다. `Packaged/`를 `.gitignore`에
   추가하고 한 번 패키징해볼 것
6. **Stage 5~8** — 스미스 체인 나머지 (`Q_Smith_002`, `Q_Smith_003`)

### 피사체 인식 설계 (예정 — 블록아웃 직후)

**해결하려는 것:** 지금 사진의 `SubjectID`는 **촬영 당시 플레이어가 있던 좌표**에서 온다.
그래서 한 레벨 안에서 "달동네를 찍었나 아파트를 찍었나"를 구분할 수 없다. 어디를 향해
찍든 그 좌표의 `PhotoSubjectID` 하나로 기록된다.

**방식 (확정): 피사체마다 컴포넌트를 붙이고, 촬영 프레임 안의 점유율로 판정한다.**

```
UGPhotoSubjectComponent              피사체 액터에 부착. SubjectID, 최소 점유율 보유
GPhotoSubjectRegistrySubsystem       BeginPlay에 자기등록 — 셔터 때 전체 액터 스캔을 피한다
UGCameraComponent::ResolveSubject()  아래 순서로 판정, FName 반환
```

1. 등록된 피사체 중 카메라 프러스텀 안에 있는 것만 추림
2. 바운즈를 화면에 투영해 **화면 점유 면적** 계산
3. 바운즈의 몇 점으로 트레이스해 **가려졌는지** 확인
4. `점유율 × 중앙 근접도`로 점수를 매겨 1등 채택
5. 기준 미달이면 `NAME_None`

중앙 라인트레이스 한 발이나 격자 트레이스로도 되지만 그 둘은 **"화면에 얼마나 크게
담겼는가"를 모른다.** 멀리 점처럼 찍힌 것과 화면을 꽉 채운 것이 동점이 된다.
앞으로 올릴 것들 — 사진 평가/등급, 스미스의 "너무 멀리서 찍었네" 류 반응,
DSLR 초점 대상 선정, 셰이드가 사진에 찍히는지 여부 — 이 전부가 이 점수 위에 얹힌다.
싸게 시작하면 그때 판정부를 다시 쓰게 된다.

**붙는 자리는 한 줄이다.** `SubjectID`라는 한 겹을 둔 덕분에 퀘스트 판정 로직은 그대로다.

```cpp
FPhotoData NewPhoto = Metadata;
// 프레임 안의 피사체가 잡히면 그쪽이 우선, 없으면 좌표 기본값을 그대로 쓴다
if (const FName Resolved = ResolveSubject(); !Resolved.IsNone())
{
    NewPhoto.SubjectID = Resolved;
}
```

> ⚠️ **좌표 방식을 지우지 말 것.** 피사체를 하나도 안 심은 레벨(들판, 주점)은 지금처럼
> 좌표값으로 동작하고, 심은 레벨만 새 방식이 이긴다. 기존 콘텐츠도 세이브도 안 건드린다.

이게 들어오면 `L_Residential_01` 하나에 `Photo_House`(골목 저층)와
`Photo_Apartment`(언덕 너머 원경)를 같이 둘 수 있다. 좌표를 나눌 필요도, 레벨을 쪼갤
필요도 없다 — 위 「촬영 대상 지정」의 *"레벨을 나눌지 피사체 조준을 도입할지"* 에 대한 답이다.

> 남은 검증: **꽃을 줍지 않고 주점 이동을 시도했을 때 실제로 거부되는지** (「Stage 0 · 퀘스트 분할」 참고)

> ⚠️ **패키지에서는 메인메뉴·설정창을 볼 수 없다.** `TAG_Widget_MainMenu` / `TAG_Widget_PressAnyKey`를
> 푸시하는 코드가 없고, `GuestPlayerController::BeginPlay`가 시작하자마자 `GameHUD`를 띄운다.
> `UGuestMainMenuWidget::OnNewGameClicked()` / `HasSaveData()`도 TODO 상태다. 프론트엔드 흐름
> 담당자에게 공유 필요.

### 기타
- 노션 「퀘스트 흐름 개요」의 `Q_S_SMT_*` 표기를 `Q_Smith_*`로 갱신
- 노션 「퀘스트 흐름 개요」에 꽃 퀘스트(`Q_Field_001`) 추가 — 현재 "게임 시작 즉시 스미스"로 되어 있어 실제 흐름과 다름
- 각 Stage별 `RequiredStoryProgress` / `StoryProgressOnComplete` 실제 값 확정 후 `DT_QuestData`와 대조
- ~~사진 목표 판정 기준 확정 (선행작업 C)~~ ✅ 장소 단위(한 좌표 = 한 피사체)로 확정
- ~~날짜 진행 트리거 확정 (선행작업 D)~~ ✅ 나레이션 종료 시점으로 확정, 실측 확인 완료
- **`WBP_Narration`의 `Anim_ScreenFadeIn` / `Anim_ScreenFadeOut`** — 없어서 현재 컷 전환이다.
  페이드가 붙으면 NPC 등장 순간이 더 자연스럽게 가려진다
- **단계 단위 스토리 진행도** — `FQuestStepData`에 `StoryProgressOnComplete` 추가
  - 지금은 `FQuestData`(퀘스트 단위)에만 있어서, 한 퀘스트 중간에 무언가를 해금하려면
    퀘스트를 쪼개는 수밖에 없다 (Stage 0의 `Q_Field_001` 분할이 그 사례)
  - 퀘스트 체인이 길어지면 "쪼개기"만으로는 관리가 어려워진다. 그때 도입할 것
- **`FOnShutterDenied` 이름 정리** — 실제로는 *이동* 거부 시에만 발생한다.
  기존 BP 바인딩을 깨지 않으려 이름을 그대로 뒀다. `BTN_Shutter`(실제로는 이동 버튼)도 같은 빚
