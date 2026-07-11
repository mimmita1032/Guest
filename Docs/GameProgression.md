# 게임 진행 흐름 (Game Progression)

이 문서는 게임 오프닝부터 실제 플레이 순서대로 어떤 일이 벌어지는지, 그리고 각 시점이 `StoryProgress`(`GQuestSubsystem`)의 몇 단계에 해당하는지를 정리한다.
목적: 만들어둔 시스템(퀘스트/대화/인벤토리/세이브 등)을 실제 플레이 가능한 한 줄기 경로로 연결하고, 아직 없는 기능을 이 문서 기준으로 파악한다.

## 사용법
- 확정된 진행 단계만 표에 채운다. 미정인 부분은 `TBD`로 남긴다.
- `StoryProgress` 값은 `FQuestData::RequiredStoryProgress` / `StoryProgressOnComplete`와 그대로 대응해야 한다.
- 구현 여부(구현됨 / 부분구현 / 미구현)를 표시해서, 이 문서만 보고 다음에 뭘 만들어야 하는지 알 수 있게 한다.

## 진행 단계표

| StoryProgress | 위치/씬 | 사건 | 플레이어 행동 | 다음 단계로 넘어가는 트리거 | 구현 상태 |
|---|---|---|---|---|---|
| 0 | 현실세계 · 들판 (`L_Field_01`) | 오프닝. 주인공이 들판에 핀 꽃을 발견한다 | 꽃을 화분에 옮겨 담는다 | 화분 들고 집으로 이동 → 씬 전환 | 구현됨 |
| 1 | 현실세계 · 집 (레벨명 TBD) | 씬 전환으로 집 도착 | 테이블에 화분을 놓는다 | 화분을 테이블에 배치 완료 | 부분구현 |

## 상세 — Stage 0: 오프닝 (현실세계 · 들판)

- **씬**: 현실세계의 들판. 주인공이 핀 꽃 한 송이를 화분에 옮겨 심는다.
- **레벨**: `L_Field_01` (`Content/Maps/Earth/Field/L_Field_01.umap`) — 기존 네이밍 규칙(`L_[지역명]`, Earth/Beyond 구분) 준용, 신규 폴더
  - ⚠️ 레벨은 에디터에서 직접 생성해야 함 (umap을 git으로 복사/생성하면 내부 경로가 깨짐 — 반드시 File → New Level로 생성 후 저장)
- **플레이어 행동**: 퀘스트 UI가 뜨고, 들판을 돌아다니다 특정 꽃과 상호작용 → 화분에 담긴 꽃이 인벤토리에 들어옴 → 퀘스트 트래커가 다음 단계(귀가)로 넘어감 (완료 아님)
- **다음 단계 트리거**: 화분 들고 집으로 이동 → 씬 전환
  - 이동 수단: 디지캠(`UGDigicamComponent`)의 레벨이동 기능 재사용 — `GSpacetimeSubsystem::ExecuteTravel` → `OpenLevel` (자세한 구조는 `project_digicam_progress` 메모리 참고)
- **필요 시스템 / 구현 현황**:
  - ✅ 꽃 줍기·인벤토리 획득·퀘스트 목표 갱신: `AGItemPickup` (기존 시스템 재사용, 신규 코드 불필요)
  - ✅ 레벨 진입 시 NPC 없이 퀘스트 자동 수락: `AGQuestAutoStarter` (`Source/Guest/Quest/WorldActor/`) — `QuestID` 지정하면 BeginPlay에 `AcceptQuest` 호출
  - ✅ 꽃 `UGItemDefinition` 데이터애셋 (`DA_Item_Flower`, `Content/Items/Definitions/Quest/`)
  - ✅ `DT_QuestData`에 오프닝 퀘스트 행 추가 — `Q_Field_001` (Step01: Collect(Flower) → ReachHome)
  - ✅ `L_Field_01`에 `AGItemPickup`(꽃) + `AGQuestAutoStarter`(QuestID=Q_Field_001) 배치
- **구현 상태**: 구현됨

## 상세 — Stage 1: 집 도착 (현실세계 · 집)

- **씬**: 디지캠으로 특정 Year/AreaCode(집)로 이동 → 도착하면 자동으로 "귀가" 목표 완료 → 테이블에 화분을 올려두면 Step 완료
- **레벨**: TBD — 에디터에서 신규 생성 필요 (umap은 git으로 만들 수 없음, File → New Level)
- **플레이어 행동**: 집에 도착 → 테이블(`AGItemPlacementPoint`)과 상호작용 → 인벤토리의 화분이 제거되고 테이블 위에 시각적으로 배치됨 → Step 완료
- **다음 단계 트리거**: Step 완료 후 NPC와 바 대화 기능으로 다음 퀘스트 시작 (기존 `ABarCustomerNPC`/`WBP_BarDialogue` 재사용, 대화 노드 QuestEventID로 다음 퀘스트 자동 수락)
- **필요 시스템 / 구현 현황**:
  - ✅ Reach 목표 자동 완료: `AGQuestReachTrigger` 신규 작성 (`Source/Guest/Quest/WorldActor/`) — TargetID 지정하면 BeginPlay에 `OnObjectiveUpdated` 브로드캐스트
  - ✅ 특정 위치에 아이템 배치: `AGItemPlacementPoint` 신규 작성 (`Source/Guest/Items/WorldActor/`) — RequiredItem 지정하면 상호작용 시 인벤토리에서 제거 후 시각화 + Place 목표 브로드캐스트
  - ✅ `EQuestObjectiveType::Place` 신규 추가
  - ⬜ (에디터 작업 필요) 집 레벨 신규 생성
  - ⬜ (에디터 작업 필요) `DT_QuestData`의 `Q_Field_001`에 Step02 추가 (Reach 목표 + Place 목표)
  - ⬜ (에디터 작업 필요) 집 레벨에 `AGQuestReachTrigger`(TargetID=귀가 ObjectiveID), 테이블 위치에 `AGItemPlacementPoint`(RequiredItem=DA_Item_Flower) 배치
  - ⬜ (에디터 작업 필요) `GSpacetimeData`에 집 Year/AreaCode 행 추가 + 디지캠에서 그 값으로 검색 가능하도록 설정
- **구현 상태**: 부분구현 (C++ 준비 완료, 레벨/콘텐츠 작업 남음)

## TODO
- Stage 0 이후 흐름 계속 이어서 정리
- 각 Stage별 `RequiredStoryProgress` / `StoryProgressOnComplete` 값을 실제 DT_QuestData 행과 매핑
- 미구현 기능 목록을 별도 섹션으로 분리할지 검토
