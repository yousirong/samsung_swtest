# [프로그래머스] 2025 카카오 하반기 1차 - 리프 노드 수 최대화

`swtest/` 의 표준입출력형 풀이를 프로그래머스 제출형으로 옮긴 것이다.
**코드는 swtest 판과 같다.** `input()`이 인자를 받고 `main()`이 `solution()`이 된 것만 다르다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/프로그래머스_2025_하반기1차_리프노드수최대화.cpp](../../swtest/프로그래머스_2025_하반기1차_리프노드수최대화.cpp) |
| 문제 | https://school.programmers.co.kr/learn/courses/30/lessons/468372 |
| 반환 타입 | `int` |
| 제출 파일 | `solution.cpp` (통째로 붙여 넣기) |
| 로컬 테스트 | `local_test.cpp` (제출하지 않음) |

## 시그니처

```cpp
int solution(int dist_limit, int split_limit)
```

## 원본 stdin을 어떻게 인자로 바꿨나

한 줄에 `dist_limit split_limit` 두 개다.

```cpp
scanf("%d %d", &dist_limit, &split_limit);   // 원본 scanf 순서 그대로
```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | 프로그래머스 템플릿대로 `#include <string>`, `#include <vector>`, `using namespace std;` |
| `input()` | `scanf`를 인자 대입으로 교체 |
| `main()` → `solution()` | `printf` → `return (int)answer` |
| 로직 함수 | 무변경. 내부 계산은 전부 `long long` |

## 이 문제만의 포인트

- **리프 수 = 1 + (2갈래 노드 수) + 2 × (3갈래 노드 수).** 리프 하나를 f갈래 분배 노드로 바꾸면 리프가 f - 1개 늘어난다.
- **층 모양은 "2층 a개 위, 3층 b개 아래"만 보면 된다.** 2^a × 3^b ≤ split_limit 인 (a, b)는 몇백 쌍뿐이다.
- **(a, b)마다 첫 3층에 놓을 노드 수 t만 고른다.** t개를 받치는 2층 부모는 최소 ceil(t/2) + ceil(t/4) + … + 1개.
  남은 예산은 +2짜리 3층에 먼저, 그다음 +1짜리 2층에 쓴다.
- **t는 이분 탐색.** "예산이 3층에 다 들어가는 가장 작은 t"와 그 바로 앞(t - 1)을 계산한다. t가 최대 10^9 이라 하나씩 볼 수 없다.
- **2층을 꽉 채우는 greedy는 틀린다.** (12, 24)에서 greedy는 18, 정답은 19. 위 층을 덜 채워 아낀 예산을 3층에 써야 한다.
- 값이 10^9 근처라 곱셈이 `int`를 넘는다. 계산은 `long long`, 반환할 때만 `int`로 바꾼다 (답은 split_limit 이하).

## 검증에 쓴 방법

- 입출력 예 4개 통과.
- 각 층의 분배 노드 수를 전부 바꿔 보는 **완전탐색**과 dist ≤ 45, split ≤ 250 전 범위(11,500개) 비교 → 전부 일치.
- 상태를 메모하는 **정확한 DP**와 dist ≤ 200, split ≤ 10^9 범위 210개 비교 → 전부 일치.
- 제출형은 swtest 판과 무작위 300개를 대조했고 `-DREPEAT_TEST` 재호출 검사도 통과했다.

## 로컬 테스트

```bash
cd programmers/2025카카오1차_리프노드수최대화
g++ -O2 -o run local_test.cpp                # solution.cpp 를 include 해서 빌드
g++ -O2 -DREPEAT_TEST -o rep local_test.cpp  # 재호출 검사
g++ -O2 -o orig ../../swtest/프로그래머스_2025_하반기1차_리프노드수최대화.cpp
echo "3 6" | ./run    # 6
```

> 제출할 때는 `solution.cpp` 를 통째로 붙여 넣는다. `local_test.cpp` 는 제출하지 않는다.
