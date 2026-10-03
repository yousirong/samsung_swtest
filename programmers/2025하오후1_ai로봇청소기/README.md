# [코드트리] 2025 하반기 오후 1번 - AI 로봇청소기

`swtest/` 의 표준입출력형 풀이를 프로그래머스 제출형으로 옮긴 것이다.
**코드는 swtest 판과 같다.** `input()`이 인자를 받고 `main()`이 `solution()`이 된 것, 그리고 **원본 버그 4개를 고친 것**만 다르다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/코드트리_2025_하반기오후1번_ai로봇청소기.cpp](../../swtest/코드트리_2025_하반기오후1번_ai로봇청소기.cpp) |
| 문제 | https://www.codetree.ai/ko/frequent-problems/samsung-sw/problems/ai-robot/description |
| 반환 타입 | `vector<int>` |
| 제출 파일 | `solution.cpp` (통째로 붙여 넣기) |
| 로컬 테스트 | `local_test.cpp` (제출하지 않음) |

## 시그니처

```cpp
vector<int> solution(vector<vector<int>> board, vector<vector<int>> cleaners, int l)
```

## 원본 stdin을 어떻게 인자로 바꿨나

`local_test.cpp` 가 원본과 똑같은 형식으로 읽어 `solution()`에 넘긴다.
즉 아래 읽기 순서가 곧 인자 순서다.

```cpp
scanf("%d %d %d", &n, &k, &l);   // 원본 scanf 순서 그대로
scanf("%d", &board[r][c]);
scanf("%d %d", &cleaners[i][0], &cleaners[i][1]);
```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | `#include <vector>` 추가, `using namespace std;` (프로그래머스 템플릿과 같은 형태) |
| `input()` | `scanf`를 인자 대입으로 교체. 전역 초기화 루프는 원본 그대로 |
| `main()` → `solution()` | `T` 루프 껍데기 제거, `printf` → 결과 배열에 담았다가 `return` |
| 로직 함수 | 무변경. 전역 배열을 쓰는 C 스타일 그대로다 |

## 원본과 달라진 곳

**원본 버그 4개를 고쳤다** (`// [버그수정]` 표시). 원본은 그대로 내면 첫 입력에서 죽기 때문에 고치지 않으면 제출본이 의미가 없다.

| # | 원본 | 고친 것 | 위치 |
|---|---|---|---|
| 1 | `scanf("%d", MAP[r][c])` — `&` 누락, 입력을 읽자마자 죽음 | `scanf` 자체가 인자 대입으로 바뀌면서 사라짐 | `input()` |
| 2 | 이웃 칸이 0 밑으로 내려가면 **자기 칸**을 0으로 만듦 → 먼지가 음수로 남음 | `MAP[nr][nc] = 0` | `solution.cpp:255` |
| 3 | 자기 칸을 청소하지 않음 (방향 고르기에서는 자기 칸을 셌다) | 자기 칸도 최대 20 지움 | `solution.cpp:240` |
| 4 | 갈 곳이 없을 때 `{ sc, sc }` 반환 → 엉뚱한 칸으로 순간이동 | `{ sr, sc }` (제자리) | `solution.cpp:180` |

> 버그 2의 재현 : 가운데 5, 상하좌우 10 인 3 x 3 격자에서 청소 직후 이웃이 `-10` 이 된다.
> `swtest/` 원본은 기록용이라 고치지 않고 `[버그]` 주석만 달아 두었다.

**반환 형식** — 원본은 테스트(L번)마다 남은 먼지 합을 한 줄씩 출력했다. 그 순서대로 `vector<int>` 로 반환한다.

## 검증

원본은 그대로는 실행되지 않으므로(버그 1), 원본에 **같은 4곳만 고친 기준본**을 따로 만들어 랜덤 입력 200개로 `cmp` 대조했고 전부 일치했다.
`-DREPEAT_TEST` 로 빌드해 같은 인자로 두 번 호출해도 결과가 같은 것(재호출 안전)까지 확인했다.
공식 예제로는 대조하지 못했다. 고친 동작이 문제 의도와 맞는지는 코드트리에 제출해 확인해야 한다.

## 로컬 테스트

```bash
cd programmers/2025하오후1_ai로봇청소기
g++ -O2 -o run local_test.cpp                # solution.cpp 를 include 해서 빌드
g++ -O2 -DREPEAT_TEST -o rep local_test.cpp  # 재호출 검사
g++ -O2 -o orig ../../swtest/코드트리_2025_하반기오후1번_ai로봇청소기.cpp
./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt
```

> 주의 : swtest 원본은 버그 1(`&` 누락) 때문에 그대로 빌드하면 입력을 읽자마자 죽고, 버그 2~4 때문에 결과도 다르다.
> 대조하려면 원본을 복사해 위 표의 4곳을 똑같이 고친 뒤 그것을 `orig` 로 빌드한다.

> 제출할 때는 `solution.cpp` 를 통째로 붙여 넣는다. `local_test.cpp` 는 제출하지 않는다.
