# G08. 후보 조합 선택 + BFS

> 상태: 🔲 미작성 — G08 그룹 복습 시작할 때 채운다.
> [G07](G07_완전탐색시뮬.md) 의 변형. 후보 수열 대신 **조합**을 뽑고, 시뮬레이션 대신 **BFS 확산**을 돌린다.

## 언제 이 유형인가
- "후보 중 정확히 M개를 골랐을 때"
- 고른 뒤 **퍼뜨리거나 / 거리를 재는** 후처리
- 후보 개수가 작다 (≤ 10~15), `C(15, 3)` 정도

## 표준 골격
```
combination(depth, start):
    if depth == M:
        reset visit / tmpMAP
        result = BFS(선택된 M개를 전부 시작점으로)
        answer = min(answer, result)
        return
    for i = start..end:
        pick[depth] = i
        combination(depth + 1, i + 1)
```

## 핵심 패턴

```c
void DFS(int depth, int start)
{
    if (depth == M)
    {
        BFS();                              // ★ 고른 M개로 확산
        int tmp = getAnswer();
        if (tmp < minAnswer) minAnswer = tmp;
        return;
    }

    for (int i = start; i < candidateCount; i++)
    {
        num_of_cases[depth] = i;
        DFS(depth + 1, i + 1);              // ★ 조합 (i+1)
    }
}

void BFS()
{
    copyMap(tmpMAP, MAP);                   // ★★ 조합마다 반드시 초기화

    // 고른 M개를 전부 시작점으로 표시하고 큐에 넣는다
    for (int i = 0; i < M; i++)
        tmpMAP[선택[i].r][선택[i].c] = 1;

    int rp, wp; rp = wp = 0;
    for (r, c) if (tmpMAP[r][c] == 1) { queue[wp].r = r; queue[wp++].c = c; }

    while (rp < wp) { ... tmpMAP[nr][nc] = tmpMAP[out]+1; ... }   // 한 번만
}

int getAnswer()
{
    int max = 0;
    for (r, c)
    {
        if (tmpMAP[r][c] == 0) return INF;   // ★ 못 덮은 칸이 있으면 이 조합 무효
        if (max < tmpMAP[r][c]) max = tmpMAP[r][c];
    }
    return max - 1;                          // 시작을 1로 뒀으므로
}
```

## 자주 쓰는 관용구
- [ ] **다중 시작점 BFS**: 고른 M개를 전부 큐에 넣고 동시에 확산 (바이러스 백신)
- [ ] BFS 없이 맨해튼 거리로 끝나는 경우도 있다 (병원 거리 최소화) — 격자 이동 제약이 없으면 BFS 낭비
- [ ] 확산 후 검증: "모든 칸이 채워졌는가" 를 반드시 확인, 아니면 그 조합은 실패
- [ ] 반복 확산형(토스트 계란틀): 변화가 없을 때까지 `while` — 종료 조건이 곧 답
- [ ] 조합마다 `visit` / `tmpMAP` 초기화 ★

## 함정 체크리스트
- [ ] 조합마다 방문 배열 초기화 누락 ★ 최빈 실수
- [ ] "모두 퍼졌는지" 검증을 빼먹고 도달 불가 조합을 정답으로 채택
- [ ] 이미 확산된 칸을 다시 세어 시간이 짧게 나옴
- [ ] 맨해튼 거리면 되는데 BFS를 돌려 시간 초과
- [ ] `answer` 초기값을 INF 로 두지 않아 최솟값 갱신 실패

## 이 유형에 속한 문제
- [ ] [2017 상반기 오후 2번 방화벽 설치하기](../review/G08/2017상오후2_방화벽설치하기.md) — 벽 3개 조합 + 확산
- [ ] [2018 상반기 오후 2번 병원 거리 최소화하기](../review/G08/2018상오후2_병원거리최소화하기.md) — 조합 + 맨해튼
- [ ] [2018 하반기 오전 2번 토스트 계란틀](../review/G08/2018하오전2_토스트계란틀.md) — 반복 연합 BFS
- [ ] [2019 상반기 오후 2번 바이러스 백신](../review/G08/2019상오후2_바이러스백신.md) — 조합 + 다중 시작점 BFS
