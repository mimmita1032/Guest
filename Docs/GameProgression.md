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

**판정 방식 (확정):** 지금은 **장소 단위**로 동작한다 — 한 좌표 = 한 피사체.
현재 촬영이 시공간 이동 직전 자동 촬영이라 "피사체를 조준한다"는 개념 자체가 없기 때문이다.

`SubjectID`라는 한 겹을 둔 덕분에, 추후 카메라에 트레이스가 생기면 **그 필드를 실제 피사체로 채우기만 하면 되고** `DT_QuestData`의 목표 데이터와 판정 로직은 그대로 둘 수 있다.

> 따라서 아파트/주택가/달동네는 현재 구조에서 **각각 다른 좌표(`DT_SpacetimeData` 행)** 로 두어야 구분된다.
> 능동 촬영이 생기면 한 레벨 안에 세 피사체를 두는 방식으로 바꿀 수 있다.

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
- **구현 상태**: 구현됨

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
  - ⬜ (에디터) `DT_QuestData`의 `Q_Field_001`에 Step02 추가 (Reach + Place)
  - ⬜ (에디터) `L_TavernMain`에 `AGQuestReachTrigger` + `AGItemPlacementPoint`(RequiredItem=DA_Item_Flower) 배치
  - ⬜ (에디터) `DT_SpacetimeData`에 주점 Year/AreaCode 행 추가
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

### 1. 사진 아이템 설계도 — 이게 없으면 촬영이 아예 안 된다
- ⬜ `DA_Item_Photo` 생성 (`UGItemDefinition`) — 위치는 `Content/Items/Definitions/` 권장
  - `ItemID` 지정 (예: `Photo`)
  - `UGItemFragmentInventory` 프래그먼트 추가 → `GridSize`(사진이 인벤토리에서 차지할 칸)와 `ItemIcon` 설정
- ⬜ 플레이어 블루프린트의 **Camera 컴포넌트 → `Photo Item Definition`** 에 위 애셋 지정
  - 미지정 시 촬영이 실패하고 `"PhotoItemDefinition 미설정"` 경고가 뜬다

### 2. 촬영 대상 지정 (사진 퀘스트용)
- ⬜ `DT_SpacetimeData`의 각 행에 **`PhotoSubjectID`** 입력
  - 그 좌표에서 찍은 사진에 기록될 식별자. 비워두면 그 사진은 어떤 퀘스트도 진행시키지 않는다
  - 스미스 1차 의뢰용으로 아파트/주택가/달동네를 **각각 다른 행**으로 만들고 서로 다른 ID를 줄 것

### 3. 시간 경과 연출
- ⬜ `DA_Narration`(꽃 배치 후 재생되는 것)의 **`Days To Advance On Finish`** 를 `1`로
- ⬜ **`Hour On Finish`** 를 `9.0` 정도로 (다음 날 아침)

### 4. 나머지 (기존 목록)
- ⬜ `WBP_Narration`에 `Anim_ScreenFadeIn` / `Anim_ScreenFadeOut` (선택 — 없으면 컷 전환)
- ⬜ `DT_QuestData`의 `Q_Field_001`에 Step02 추가 (Reach + Place)
- ⬜ `L_TavernMain`에 `AGQuestReachTrigger` + `AGItemPlacementPoint` 배치
- ⬜ `DT_SpacetimeData`에 주점 좌표 행 추가

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
- 메인 퀘스트 체인 (노션상 미기획)
- 히든 퀘스트 (노션상 미기획)

## TODO
- 노션 「퀘스트 흐름 개요」의 `Q_S_SMT_*` 표기를 `Q_Smith_*`로 갱신
- 노션 「퀘스트 흐름 개요」에 꽃 퀘스트(`Q_Field_001`) 추가 — 현재 "게임 시작 즉시 스미스"로 되어 있어 실제 흐름과 다름
- 각 Stage별 `RequiredStoryProgress` / `StoryProgressOnComplete` 실제 값 확정 후 `DT_QuestData`와 대조
- 사진 목표 판정 기준 확정 (선행작업 C)
- 날짜 진행 트리거 확정 (선행작업 D)
