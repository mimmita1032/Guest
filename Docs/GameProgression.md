# 게임 진행 흐름 (Game Progression)

이 문서는 게임 오프닝부터 실제 플레이 순서대로 어떤 일이 벌어지는지, 그리고 각 시점이 `StoryProgress`(`GQuestSubsystem`)의 몇 단계에 해당하는지를 정리한다.
목적: 만들어둔 시스템(퀘스트/대화/인벤토리/세이브 등)을 실제 플레이 가능한 한 줄기 경로로 연결하고, 아직 없는 기능을 이 문서 기준으로 파악한다.

## 사용법
- 확정된 진행 단계만 표에 채운다. 미정인 부분은 `TBD`로 남긴다.
- `StoryProgress` 값은 `FQuestData::RequiredStoryProgress` / `StoryProgressOnComplete`와 그대로 대응해야 한다.
- 구현 여부(구현됨 / 부분구현 / 미구현)를 표시해서, 이 문서만 보고 다음에 뭘 만들어야 하는지 알 수 있게 한다.

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
| 1 | 주점 (`L_TavernMain`) | 귀가. 주점에 화분을 놓는다 | 테이블에 화분 배치 | 배치 완료 → 나레이션 재생 | 부분구현 |
| 2 | 주점 (`L_TavernMain`) | 나레이션 종료 후 **하루 경과**. 스미스 첫 방문 | 맥주 2잔 접객 → 의뢰 수주 | 대화 완료 → `Q_Smith_001` 수락 | 미구현 |
| 3 | 현실 · 주택가 (`L_Residential_01`) | 평범한 건물 촬영 | 아파트/주택가/달동네 3종 촬영 | 사진 3장 확보 후 귀가 | 미구현 |
| 4 | 주점 (`L_TavernMain`) | 사진 전달. 스미스가 "영감이 안 온다"며 보수 지불 | 스미스와 대화 | `Q_Smith_001` 완료 | 미구현 |
| 5 | 주점 (`L_TavernMain`) | 스미스 재방문. 독특한 건물 의뢰 | 대화 | `Q_Smith_002` 수락 | 미구현 |
| 6 | 현실 · 랜드마크 3종 | 독특한 건물 촬영 | 박물관/전통건축/스카이스파이어 촬영 | 사진 전달 → 스미스가 스카이스파이어 선택 | 미구현 |
| 7 | 현실 · 스카이스파이어 (`L_Landmark_SkySpire_01`) | 추가 의뢰 — 입체 모형 수거 | 내부 전망대에서 모형 획득 | 모형 들고 귀가 | 미구현 |
| 8 | 주점 (`L_TavernMain`) | 모형 전달. 스미스 보수 지불 후 한동안 이탈 | 스미스와 대화 | `Q_Smith_003` 완료 | 미구현 |
| 9 | 중앙 도시 (`L_CitySquare`) | **엔딩** — 스미스가 완성된 홀로그램 설계도를 보여주며 감사 인사 | 관람 | 데모 종료 화면 | 미구현 |

---

## ⚠️ 선행 시스템 작업

콘텐츠 배치보다 **먼저** 끝내야 하는 것들. 나중에 하면 이미 만든 퀘스트/세이브를 다시 손봐야 한다.

> **진행 상황 (2026-07-26): A·B·C·D 코드 작업 완료.** 브랜치 `feat/item-instance-data`.
> 남은 것은 **에디터 작업**(맨 아래 「에디터에서 해야 할 일」)과 **E(데모 엔딩 화면)**.

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
→ Stage 1의 "꽃 배치 → 나레이션 → 하루 뒤 스미스"가 **코드 수정 없이 데이터만으로** 완성된다.

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
  - ✅ `DT_QuestData`의 `Q_Field_001` (Step01: Collect(Flower) → ReachHome)
  - ✅ `L_Field_01`에 `AGItemPickup`(꽃) + `AGQuestAutoStarter` 배치
  - ⬜ **(에디터)** `Q_Field_001`을 꽃 줍기까지로 축소 + `StoryProgressOnComplete = 1` (아래 「퀘스트 분할」)
- **구현 상태**: 구현됨

### 퀘스트 분할 — 주점 이동 게이팅 (확정)

**문제**: `Tavern` 좌표의 `RequiredStoryProgress`가 `0`이라 **꽃을 줍지 않고도 주점에 갈 수 있다.**
퀘스트 흐름상 주점 이동 조건은 "꽃을 인벤토리에 넣는 것"인데 그게 강제되지 않는다.

**구조적 제약**: `StoryProgressOnComplete`는 **퀘스트 단위** 필드다(`FQuestData`). 단계 단위가 아니다.
그런데 원래 설계는 꽃 줍기와 주점 도착·배치가 **같은 퀘스트의 다른 단계**였다.
그러면 진행도가 오르는 시점이 주점에서 화분을 놓은 *뒤*라서, 주점 이동을 막는 데 쓸 수 없다.

**채택안 — 퀘스트를 둘로 쪼갠다** (데이터만으로 해결):

| 퀘스트 | 범위 | 핵심 필드 |
|---|---|---|
| `Q_Field_001` | 꽃 줍기까지 | `StoryProgressOnComplete = 1`, `NextQuestID = Q_Field_002` |
| `Q_Field_002` | 귀가 + 화분 배치 | `RequiredStoryProgress = 1`, `NarrationOnComplete` = 나레이션 애셋 |

그리고 `DT_SpacetimeData`의 `Tavern` 행에 **`RequiredStoryProgress = 1`**.
→ 꽃을 줍기 전에는 주점 좌표가 `Locked`으로 뜨고 이동이 거부된다.

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
  - ⬜ (에디터) `DT_QuestData`에 **`Q_Field_002`** 신설 (Reach + Place) — 「퀘스트 분할」 참고
  - ⬜ (에디터) `L_TavernMain`에 `AGQuestReachTrigger` + `AGItemPlacementPoint`(RequiredItem=DA_Item_Flower) 배치
  - ✅ (에디터) `DT_SpacetimeData`의 `Tavern` 행 (2026 / 0)
  - ⬜ (에디터) `Tavern` 행의 `RequiredStoryProgress`를 `1`로 — 꽃을 줍기 전에는 주점에 못 가게
  - ⬜ (에디터) 나레이션 애셋 본문 작성 — "다음 날 아침…" 문구로 하루 경과를 연출
  - ⬜ 날짜 시스템(선행작업 D) 연동
- **구현 상태**: 부분구현 (C++ 준비 완료, 콘텐츠 작업 남음)

## 상세 — Stage 2~4: 스미스 첫 방문 (`Q_Smith_001` 평범한 것들의 기록)

- **씬**: 주점. 스미스가 첫 손님으로 들어온다.
- **대화 흐름** (노션 「스미스」 문서 기준):
  1. 호기롭게 들어와 맥주 한 잔 주문 → 원샷 (호탕한 성격 제시)
  2. 두 번째 맥주를 주문하며 자신이 건축가임을 밝힘
  3. 주인공이 주점을 물려받았다는 것을 언급, 마음에 드는 건물이 없었다고 한탄
  4. 현실 세계의 건물들을 보면 영감이 떠오를지도 모르겠다며 의뢰
- **수거 대상**: 아파트 / 주택가 / 달동네 사진 3종 — 노션 레벨 DB상 촬영 구역은 `L_Residential_01` ("현실 세계 — 주택가 구역", 미시작)
- **결말**: 스미스는 "이미 비슷한 걸 지어봤다"며 영감을 못 얻지만, 다음엔 독특한 것을 달라고 하며 **보수 지불**
- **구현 현황**:
  - ✅ NPC 등장 게이팅 — `AGuestNPCBase::RequiredStoryProgress` / `ApplyStoryProgressVisibility()`
  - ✅ 바 대화 시스템 — `ABarCustomerNPC`, `WBP_BarDialogue`
  - ⬜ 선행작업 A·B·C (사진 아이템화 + Photo 목표)
  - ⬜ (에디터) `DT_QuestData`에 `Q_Smith_001` 행 — 현재 있는 행은 스텁 상태
  - ⬜ (에디터) 스미스 대화 노드 작성 (`DT_Smith`, `DA_Smith_Dialogue`)
  - ⬜ (에디터) `L_Residential_01` 촬영 구역 제작 — 아파트/주택가/달동네 3개소
  - ⬜ 판정 기준에 따라 `DT_SpacetimeData` 구역 분리 여부 결정 (선행작업 C 참고)
- **구현 상태**: 미구현

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
- ⬜ 아파트/달동네 — 레벨을 나눌지 피사체 조준을 도입할지 미정 (「C. 판정 방식」 참고)
  - 지금 구조에서는 **각각 다른 행**이어야 구분된다. 다만 같은 레벨을 세 행이 가리키면
    플레이어 눈에는 같은 풍경을 세 번 찍는 것이 된다

### 3. 시간 경과 연출
- ⬜ `DA_Narration`(꽃 배치 후 재생되는 것)의 **`Days To Advance On Finish`** 를 `1`로
- ⬜ **`Hour On Finish`** 를 `9.0` 정도로 (다음 날 아침)

### 4. 퀘스트 분할 (주점 이동 게이팅 — 「Stage 0 · 퀘스트 분할」 참고)
- ⬜ `DT_QuestData`의 `Q_Field_001`을 **꽃 줍기까지로 축소**
  - `StoryProgressOnComplete = 1`, `NextQuestID = Q_Field_002`
- ⬜ `DT_QuestData`에 **`Q_Field_002`** 신설 (Reach + Place)
  - `RequiredStoryProgress = 1`, `NarrationOnComplete` = 나레이션 애셋
- ⬜ `DT_SpacetimeData`의 `Tavern` 행 → **`RequiredStoryProgress = 1`**

### 5. 나머지 (기존 목록)
- ⬜ `WBP_Narration`에 `Anim_ScreenFadeIn` / `Anim_ScreenFadeOut` (선택 — 없으면 컷 전환)
- ⬜ `L_TavernMain`에 `AGQuestReachTrigger` + `AGItemPlacementPoint` 배치

> ⚠️ **기존 세이브의 사진은 유실된다.** `SavedPhotos` 배열을 제거하고 인벤토리로 일원화했기 때문이다.
> 개발 단계라 마이그레이션 코드는 넣지 않았다. 새로 시작하면 문제없다.

## 작업 순서 권고

1. ~~**선행 시스템** — A(개체별 데이터) → B(사진 아이템화) → C(Photo 목표) → D(날짜)~~ ✅ 코드 완료
2. **에디터 작업 1~3** — 특히 `DA_Item_Photo`가 없으면 촬영 자체가 동작하지 않는다
3. **Stage 1 마무리** — 주점 배치 + 나레이션까지 한 줄기로 플레이 가능하게
4. **Stage 2~4** — 스미스 첫 방문. 여기서 핵심 루프가 처음으로 완주된다
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

### 다음 할 일 — 에디터 (Stage 1 완주까지)
촬영·이동 파이프라인은 실측 확인 완료. 남은 것은 이 셋이고, 순서대로 하면 Stage 1이 한 줄기로 이어진다.

1. **`DA_Narration` 시간 경과 설정** — `Days To Advance On Finish = 1`, `Hour On Finish = 9.0`
   → 「에디터에서 해야 할 일 · 3」
2. **퀘스트 분할** — `Q_Field_001` 축소(`StoryProgressOnComplete = 1`) + `Q_Field_002` 신설 + `Tavern` 행 `RequiredStoryProgress = 1`
   → 「에디터에서 해야 할 일 · 4」, 배경은 「Stage 0 · 퀘스트 분할」
3. **`L_TavernMain` 배치** — `AGQuestReachTrigger` + `AGItemPlacementPoint`(RequiredItem = `DA_Item_Flower`)
   → 「에디터에서 해야 할 일 · 5」

### 기타
- 노션 「퀘스트 흐름 개요」의 `Q_S_SMT_*` 표기를 `Q_Smith_*`로 갱신
- 노션 「퀘스트 흐름 개요」에 꽃 퀘스트(`Q_Field_001`) 추가 — 현재 "게임 시작 즉시 스미스"로 되어 있어 실제 흐름과 다름
- 각 Stage별 `RequiredStoryProgress` / `StoryProgressOnComplete` 실제 값 확정 후 `DT_QuestData`와 대조
- 사진 목표 판정 기준 확정 (선행작업 C)
- 날짜 진행 트리거 확정 (선행작업 D)
- **단계 단위 스토리 진행도** — `FQuestStepData`에 `StoryProgressOnComplete` 추가
  - 지금은 `FQuestData`(퀘스트 단위)에만 있어서, 한 퀘스트 중간에 무언가를 해금하려면
    퀘스트를 쪼개는 수밖에 없다 (Stage 0의 `Q_Field_001` 분할이 그 사례)
  - 퀘스트 체인이 길어지면 "쪼개기"만으로는 관리가 어려워진다. 그때 도입할 것
- **`FOnShutterDenied` 이름 정리** — 실제로는 *이동* 거부 시에만 발생한다.
  기존 BP 바인딩을 깨지 않으려 이름을 그대로 뒀다. `BTN_Shutter`(실제로는 이동 버튼)도 같은 빚
