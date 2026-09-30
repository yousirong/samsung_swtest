# [프로그래머스] 2025 카카오 하반기 2차 - 선인장 숨기기

`swtest/` 의 표준입출력형 풀이를 프로그래머스 제출형으로 옮긴 것이다.
**코드는 swtest 판과 같다.** `input()`이 인자를 받고 `main()`이 `solution()`이 된 것만 다르다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/프로그래머스_2025_하반기2차_선인장숨기기.cpp](../../swtest/프로그래머스_2025_하반기2차_선인장숨기기.cpp) |
| 문제 | https://school.programmers.co.kr/learn/courses/30/lessons/468379 |
| 반환 타입 | `vector<int>` (구역의 왼쪽 위 [행, 열]) |
| 제출 파일 | `solution.cpp` (통째로 붙여 넣기) |
| 로컬 테스트 | `local_test.cpp` (제출하지 않음) |

## 시그니처

```cpp
vector<int> solution(int m, int n, int h, int w, vector<vector<int>> drops)
```

## 원본 stdin을 어떻게 인자로 바꿨나

`local_test.cpp` 가 원본과 똑같은 형식으로 읽어 `solution()`에 넘긴다.
즉 아래 읽기 순서가 곧 인자 순서다.

```cpp
scanf("%d %d %d %d", &m, &n, &h, &w);   // 원본 scanf 순서 그대로
scanf("%d", &k);
scanf("%d %d", &drops[i][0], &drops[i][1]);
```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | `#include <vector>` 추가, `using namespace std;` (프로그래머스 템플릿과 같은 형태) |
| `input()` | `scanf`를 인자 대입으로 교체 |
| `main()` → `solution()` | `printf` → 배열에 담아 `return` |
| 로직 함수 | 무변경. 전역 고정 배열을 쓰는 C 스타일 그대로다 |


## 이 문제만의 포인트

- 문제를 **"모든 h x w 창의 최솟값 중 최댓값"** 으로 바꾸면 끝난다.
  칸마다 `dropTime`(비가 떨어진 순번, 없으면 INF)을 적어 두면
  선인장을 (r, c)에 놓았을 때 비를 맞는 순간이 그 창의 최솟값이다.
  "비를 아예 안 맞는 자리"는 최솟값이 INF인 자리라서 따로 처리할 것이 없다.
- **2차원 슬라이딩 윈도 최솟값**을 두 번에 나눠 구한다.
  가로 W로 한 번(`slideRow`), 그 결과를 세로 H로 한 번(`slideColumn`).
  각 단계가 단조 덱이라 전체가 O(m x n)이다. 창마다 h x w 칸을 다 보면 시간이 터진다.
- **2차원 배열을 쓸 수 없다.** m, n이 각각 500,000까지 가므로 `[500000][500000]`은 불가능하다.
  대신 `m x n <= 500,000` 조건을 이용해 `at(r, c) = r * N + c` 로 한 줄에 펴서 담는다.
- 동점 처리는 행 우선으로 훑으며 "더 클 때만" 갱신하는 것으로 끝난다
  (가장 위쪽 행 -> 가장 왼쪽 열).

## 검증에 쓴 방법

- 문제의 입출력 예 6개를 전부 통과했다 (제출형을 채점기처럼 따로 링크해서도 확인했다).
- 12 x 12 이하 무작위 격자 300개를 **완전탐색 구현**과 비교해 전부 일치했다.
- 최대 크기(m x n = 500,000)에서 0.3초 안에 끝난다.
  1000 x 500, 500000 x 1, 1 x 500000 세 모양 모두 확인했다.

## 로컬 테스트

```bash
cd programmers/2025카카오2차_선인장숨기기
g++ -O2 -o run local_test.cpp                # solution.cpp 를 include 해서 빌드
g++ -O2 -DREPEAT_TEST -o rep local_test.cpp  # 재호출 검사
g++ -O2 -o orig ../../swtest/프로그래머스_2025_하반기2차_선인장숨기기.cpp
./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt
```

> 제출할 때는 `solution.cpp` 를 통째로 붙여 넣는다. `local_test.cpp` 는 제출하지 않는다.
