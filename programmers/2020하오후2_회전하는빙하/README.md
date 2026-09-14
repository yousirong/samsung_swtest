# 2020 하반기 오후 2번 — 회전하는 빙하 (프로그래머스 함수형)

2^N × 2^N 격자를 2^L 블록 단위로 회전시키고 얼음을 녹이기를 Q번 반복한 뒤,
남은 얼음의 총합과 가장 큰 덩어리의 크기를 구한다.

- 원본 : [swtest/코드트리_2020_하반기오후2번_회전하는빙하.cpp](../../swtest/코드트리_2020_하반기오후2번_회전하는빙하.cpp)
- 복습 노트 : [docs/review/G06/2020하오후2_회전하는빙하.md](../../docs/review/G06/2020하오후2_회전하는빙하.md)
- 원문제 : [코드트리 회전하는 빙하](https://www.codetree.ai/frequent-problems/samsung-sw/problems/rotating-glacier) (백준 20058 "마법사 상어와 파이어스톰"과 같은 문제)

## 시그니처

```cpp
std::vector<int> solution(int n, std::vector<std::vector<int>> board, std::vector<int> levels)
```

## stdin → 인자 대응

| 원본 stdin | 원본 코드 | solution |
|---|---|---|
| 1행 `N Q` | `scanf("%d %d", &N, &Q)` | `n`. **`Q`는 인자 아님** — `levels.size()`로 대신한다 |
| 이어서 (2^N)×(2^N) 격자 | `scanf("%d", &MAP[r][c])` | `board[r-1][c-1]` — **인자는 0-based, 전역은 1-based** |
| 이어서 `L` Q개 | `scanf("%d", &L[q])` | `levels[q]` |

> **격자 크기가 `n`이 아니라 `2^n`이다.** 하네스에서도 `int side = 1 << n;` 으로 읽는다. 이걸 `n × n`으로 읽으면 입력 파싱부터 어긋난다.

## 답이 2개일 때 — `std::vector<int>`로 묶는다

원본은 두 값을 각각 다른 줄에 찍었다.

```c
printf("%d\n%d\n", iceCount, groupCount);
```

함수형에서는 반환값이 하나여야 하므로 `vector<int>`에 **원본 출력 순서 그대로** 담는다.

```cpp
return std::vector<int>{ iceCount, groupCount };
```

같은 규칙을 쓰는 문제 : 2017 하오후2 연산자배치(min, max), 2020 상오전1 2차원테트리스(점수, 남은 블록), 40 성곽(3개).

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | `#include <vector>` 추가 |
| `input()` | `input(int n, const vector<vector<int>>&, const vector<int>&)` — `scanf` 3곳을 인자 대입으로 |
| `main()` → `solution()` | `T = 1; for (tc...)` 껍데기 제거, `printf` → `return` |
| 로직 | **무변경** (`rotate` `meltIce` `simulate` `BFS` `getIceCount` `getGroupCount` 전부 그대로) |

## 재호출 주의점

추가 초기화가 필요 없다.

- `MAP` — `input()`이 `1..MAP_SIZE` 전 범위를 덮어쓴다
- `visit` — `getGroupCount()`가 BFS 직전에 매번 false로 민다
- `tmpMAP` — `rotate()`가 쓰기 전에 항상 먼저 채운다

> `-DREPEAT_TEST` 로 300케이스 검사 통과.

## ⚠️ 원본에 남아 있는 버그 (고치지 않음)

`rotate()`가 부분 격자를 **사분면 자리바꿈**으로 처리한다. `size`가 2일 때는 90도 회전과 결과가 같지만,
**4 이상이면 다르다.** 즉 명령의 `L`이 2 이상이면 틀린 답이 나온다.

```
4 x 4 예시
     1  2  3  4        올바른 90도 회전       현재 코드
     5  6  7  8          13  9  5  1           9 10  1  2
     9 10 11 12          14 10  6  2          13 14  5  6
    13 14 15 16          15 11  7  3          11 12  3  4
                         16 12  8  4          15 16  7  8
```

원본 파일 상단 주석에 같은 내용과 고치는 방법이 이미 적혀 있다.
이 변환본은 **원본과의 동치성**을 보장하는 것이 목적이라 그대로 두었다.
고치려면 사분면 이동 4개를 아래 한 줄로 바꾸면 된다.

```cpp
for (int r = 0; r < size; r++)
    for (int c = 0; c < size; c++)
        MAP[sr + r][sc + c] = tmpMAP[size - 1 - c][r];
```

## 로컬 테스트

```bash
cd programmers/2020하오후2_회전하는빙하
g++ -O2 -DLOCAL_TEST -o run solution.cpp
printf '1 1\n1 2\n3 4\n0\n' | ./run
```

원본과 대조:

```bash
g++ -O2 -o orig ../../swtest/코드트리_2020_하반기오후2번_회전하는빙하.cpp
printf '1 1\n1 2\n3 4\n0\n' > in.txt
./orig < in.txt > a.out; ./run < in.txt > b.out; cmp a.out b.out
```

> 제출할 때는 `solution.cpp` 맨 아래 `#ifdef LOCAL_TEST` 블록을 빼고 복사한다.
