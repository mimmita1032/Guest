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

### A. 인벤토리 개체별 데이터 (per-instance data) — 최우선

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

- ⬜ `UGItemInstance`에 개체별 데이터 확장 지점 추가
- ⬜ `FGuestSavedInventoryEntry`에 개체별 데이터 직렬화 추가
- ⬜ 기존 세이브 호환 처리 (개체 데이터 없는 구버전 엔트리 허용)

### B. 사진의 아이템화

**목표: 촬영하면 사진이 인벤토리 아이템으로 들어가고, 디지캠 「찍은 사진」 메뉴에서도 확인된다.**

현재는 사진과 아이템이 완전히 분리된 두 세계다.

| | 현재 사진 | 현재 아이템 |
|---|---|---|
| 저장소 | `GPhotoLibrarySubsystem`의 `TArray<FPhotoData>` | `GInventoryComponent` 그리드 |
| 정체 | 구조체(값) | `UGItemInstance`(오브젝트) |
| 세이브 | `SavedPhotos` 별도 배열 | `SavedInventory` |
| UI | 디지캠 갤러리 탭 | 인벤토리 그리드 |

- ⬜ 사진 설계도 `DA_Item_Photo` 1개 생성 (모든 사진이 공유)
- ⬜ 촬영 시 `FPhotoData`를 개체 데이터로 들고 있는 인스턴스를 인벤토리에 생성
- ⬜ 디지캠 갤러리를 **인벤토리에서 사진 태그만 필터링한 뷰**로 전환
- ⬜ `SavedPhotos` 별도 배열 제거 (인벤토리 세이브로 일원화)

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

### C. Photo 퀘스트 목표 타입

스미스 체인 3개가 전부 사진 수집인데 현재 목표 타입에 사진이 없다.

```cpp
enum class EQuestObjectiveType : uint8 { Talk, Collect, Reach, Place };  // Photo 없음
```

- ⬜ `EQuestObjectiveType::Photo` 추가
- ⬜ 촬영 시 `GQuestSubsystem::OnObjectiveUpdated` 브로드캐스트 연결
- ⬜ **판정 기준 TBD** — 아래 참고

**판정 기준 미결:** `FPhotoData`는 "어디서 찍었는지"만 기록하고 "무엇을 찍었는지"는 모른다. `GCameraComponent`도 렌더타겟만 다루고 조준 대상 감지가 없다.
- 장소 기준으로 가면 → 아파트/주택가/달동네를 **각각 다른 `AreaCode`로 분리**해야 구분된다 (한 레벨 안에 3개를 두면 판정 불가)
- 피사체 기준으로 가면 → 카메라에 트레이스 + 피사체 ID + 조준 UI 신규 작업

사진이 아이템이 되면 개체 데이터에 피사체 ID를 넣는 것 자체는 자연스러워지므로, **B 작업 후 다시 판단한다.**

### D. 날짜 시스템

Stage 1→2의 "하루 후"를 표현할 날짜 개념이 없다. `GSpacetimeSubsystem`은 시각(`CurrentHour`)만 관리한다.

- ⬜ `GSpacetimeSubsystem`에 날짜(Day) 카운터 추가
- ⬜ 세이브 연동 (`SavedWorldHour`와 같은 방식)
- ⬜ 날짜를 진행시키는 트리거 정의 (귀가 시? 취침? 퀘스트 완료 시?) — TBD

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

## 작업 순서 권고

1. **선행 시스템** — A(개체별 데이터) → B(사진 아이템화) → C(Photo 목표) → D(날짜)
2. **Stage 1 마무리** — 주점 배치 + 나레이션까지 한 줄기로 플레이 가능하게
3. **Stage 2~4** — 스미스 첫 방문. 여기서 핵심 루프가 처음으로 완주된다
4. **Stage 5~8** — 스미스 체인 나머지
5. **Stage 9 + E** — 엔딩과 데모 종료 화면

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
