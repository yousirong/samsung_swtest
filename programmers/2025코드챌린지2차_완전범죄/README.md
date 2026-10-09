# [프로그래머스] 2025 코드챌린지 2차 예선 - 완전범죄

`swtest/` 의 표준입출력형 풀이를 프로그래머스 제출형으로 옮긴 것이다.
**코드는 swtest 판과 같다.** `input()`이 인자를 받고 `main()`이 `solution()`이 된 것만 다르다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/프로그래머스_2025_코드챌린지2차예선_완전범죄.cpp](../../swtest/프로그래머스_2025_코드챌린지2차예선_완전범죄.cpp) |
| 문제 | https://school.programmers.co.kr/learn/courses/30/lessons/389480 |
| 반환 타입 | `int` |
| 제출 파일 | `solution.cpp` (통째로 붙여 넣기) |
| 로컬 테스트 | `local_test.cpp` (제출하지 않음) |

## 시그니처

```cpp
int solution(vector<vector<int>> info, int n, int m)
```

## 원본 stdin을 어떻게 인자로 바꿨나

첫 줄 `물건수 n m`, 이어서 물건마다 `A흔적 B흔적` 한 줄씩. 물건 수는 `info.size()`로 대신한다.

```cpp
scanf("%d %d %d", &cnt, &n, &m);   // 원본 scanf 순서 그대로
scanf("%d %d", &info[i][0], &info[i][1]);
```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | 프로그래머스 템플릿대로 `#include <string>`, `#include <vector>`, `using namespace std;` |
| `input()` | `scanf`를 인자 대입으로 교체. `visit` 초기화는 원본 그대로 |
| `main()` → `solution()` | `printf` → `return` |
| 로직 함수 | 무변경. DFS + 방문 체크 그대로 |

## 이 문제만의 포인트

- **물건마다 "A가 훔친다 / B가 훔친다" 두 갈래 DFS.** 그대로면 2^40이라 너무 많다.
- **`visit[L][a][b]`로 같은 상태를 다시 보지 않는다.** "L번째 물건까지 정했고 흔적이 a, b"인 상태는
  어떻게 왔든 앞으로가 똑같다. 상태는 41 × 120 × 120 ≈ 59만 개뿐이다.
- **`a >= answer`면 가지치기.** answer는 줄기만 하므로 방문 체크와 같이 써도 답을 놓치지 않는다.
- 흔적이 n **이상**이면 붙잡힌다 → 살아 있는 범위는 `a < n`, `b < m`.
- 못 찾으면 `-1`.

## 검증에 쓴 방법

- 입출력 예 4개 통과.
- 물건 16개 이하 무작위 3,000개를 2^k 완전탐색과 비교 → 전부 일치 (같은 인자로 두 번 호출한 결과도 같았다).
- 최대 크기(물건 40개, n = m = 120)에서 0.1초 미만.

## 로컬 테스트

```bash
cd programmers/2025코드챌린지2차_완전범죄
g++ -O2 -o run local_test.cpp                # solution.cpp 를 include 해서 빌드
g++ -O2 -DREPEAT_TEST -o rep local_test.cpp  # 재호출 검사
printf "3 4 4\n1 2\n2 3\n2 1\n" | ./run      # 2
```

> 제출할 때는 `solution.cpp` 를 통째로 붙여 넣는다. `local_test.cpp` 는 제출하지 않는다.
