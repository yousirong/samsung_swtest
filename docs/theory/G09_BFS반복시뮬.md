# G09. 최단거리 BFS 반복 시뮬

> 상태: 🔲 미작성 — G09 그룹 복습 시작할 때 채운다.
> 완전탐색이 없다. 대신 **매 턴 BFS를 다시 돌린다.** 턴 수 × BFS 비용이 곧 복잡도다.

## 언제 이 유형인가
- "가장 가까운 ~를 찾아 이동한다"를 M번 반복
- 이동할 때마다 지도 상태가 바뀐다 (승객 하차, 몬스터 제거, 폭탄 제거)
- 동점 처리 규칙이 명시된다 → **"거리 → 행 → 열" 우선순위**

## 표준 골격
```
for turn = 1..M:
    target = BFS_findNearest(현재 위치)   # 동점이면 우선순위 규칙
    if target 없음: break / 실패
    cost = BFS_distance(현재 → target)
    상태 갱신 (제거 / 소모 / 레벨업)
    현재 위치 = target
```

## 핵심 패턴

```c
// 매 턴 BFS 를 다시 돌린다
for (int turn = 1; turn <= M; turn++)
{
    // ★★ visit 초기화를 빼먹으면 두 번째 턴부터 아무것도 못 찾는다
    for (r, c) visit[r][c] = 0;

    // BFS 로 거리 계산
    queue[wp].r = cur.r;  queue[wp++].c = cur.c;
    visit[cur.r][cur.c] = 1;                       // 시작 1 → 실제 거리는 -1
    while (rp < wp) { ... visit[nr][nc] = visit[out]+1; ... }

    // 대상 선택 — 두 가지 방식
    ...
}
```

### 동점 처리 — 두 가지 방식

```c
// (가) BFS 후 격자를 (r,c) 순서로 스캔  ← 권장
//     스캔 순서 자체가 "위쪽 우선 → 왼쪽 우선" 을 만든다
for (int r = 1; r <= N; r++)
    for (int c = 1; c <= N; c++)
    {
        if (대상이 아님 || visit[r][c] == 0) continue;
        if (visit[r][c] < minTime)              // ★ < 하나면 끝 (<= 쓰면 반대가 된다)
        { minTime = visit[r][c]; tr = r; tc = c; }
    }

// (나) BFS 안에서 (거리, r, c) 사전순 3중 비교
if ((visit[nr][nc] <  minDistance)
 || (visit[nr][nc] == minDistance && nr <  minR)
 || (visit[nr][nc] == minDistance && nr == minR && nc < minC))
{ ... }
```

**(가)가 조건 하나로 끝나 실수가 적다.** 단 우선순위 항목의 방향이 뒤섞이면
([색깔 폭탄](../review/G09/2021상오전2_색깔폭탄.md)처럼) **비교 함수로 분리**해야 한다.

```c
// (다) 항목마다 방향이 다를 때 — 비교 함수로 분리
bool isPriority(BOMB a, BOMB b)
{
    if (a.total != b.total) return a.total > b.total;   // 크게
    if (a.red   != b.red)   return a.red   < b.red;     // 작게
    if (a.r     != b.r)     return a.r     > b.r;       // 크게
    return a.c < b.c;                                    // 작게
}
// ★ 초기값도 각 항목 반대 방향으로 잡는다
```

## 자주 쓰는 관용구
- [ ] 동점 우선순위: BFS 로 거리를 다 구한 뒤 **전체 스캔**하며 `(거리, r, c)` 사전순 비교
  — 큐에서 먼저 나온 것을 채택하면 방향 배열 순서에 의존해 틀린다
- [ ] BFS 를 두 번 쓰는 구조 (대상 찾기 / 대상까지 거리) — 자율주행 전기차
- [ ] 매 턴 `visit` 초기화, 큐 head/tail 리셋
- [ ] 도달 불가 판정을 명확히 (`INF` 유지 여부)
- [ ] 상태 변화 후 지도를 반드시 갱신하고 다음 BFS 를 돌린다

## 함정 체크리스트
- [ ] 동점을 큐 순서로 처리 ★ 최빈 실수 — 반드시 전체 스캔 비교
- [ ] 턴마다 `visit` 초기화 누락
- [ ] "도달 불가" 와 "거리 0" 구분 실패
- [ ] 소모값(배터리/레벨)의 갱신 시점이 문제 규칙과 어긋남
- [ ] 시작 칸 자신을 대상으로 잡아 무한 루프

## 이 유형에 속한 문제
- [ ] [2018 하반기 오후 2번 전투로봇](../review/G09/2018하오후2_전투로봇.md) — 우선순위 선택 + 레벨업
- [ ] [2020 상반기 오후 2번 자율주행 전기차](../review/G09/2020상오후2_자율주행자동차.md) — BFS 2회 × M명
- [ ] [2021 상반기 오전 2번 색깔 폭탄](../review/G09/2021상오전2_색깔폭탄.md) ⚠︎ 소스에 주석 없음 — 코드 정독 필요
