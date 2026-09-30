# [프로그래머스] 2025 카카오 하반기 2차 - 힌트 스테이지

`swtest/` 의 표준입출력형 풀이를 프로그래머스 제출형으로 옮긴 것이다.
**코드는 swtest 판과 같다.** `input()`이 인자를 받고 `main()`이 `solution()`이 된 것만 다르다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/프로그래머스_2025_하반기2차_힌트스테이지.cpp](../../swtest/프로그래머스_2025_하반기2차_힌트스테이지.cpp) |
| 문제 | https://school.programmers.co.kr/learn/courses/30/lessons/468377 |
| 반환 타입 | `int` |
| 제출 파일 | `solution.cpp` (통째로 붙여 넣기) |
| 로컬 테스트 | `local_test.cpp` (제출하지 않음) |

## 시그니처

```cpp
int solution(vector<vector<int>> cost, vector<vector<int>> hint)
```

## 원본 stdin을 어떻게 인자로 바꿨나

`local_test.cpp` 가 원본과 똑같은 형식으로 읽어 `solution()`에 넘긴다. 번들 줄은 `가격 + 힌트권 k장` 이다.

```cpp
scanf("%d", &n);   // 원본 scanf 순서 그대로
scanf("%d", &cost[i][j]);
scanf("%d", &k);
scanf("%d", &hint[i][j]);
```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | `#include <vector>` 추가, `using namespace std;` (프로그래머스 템플릿과 같은 형태) |
| `input()` | `scanf`를 인자 대입으로 교체. 번들 크기는 `hint[i].size() - 1` 로 구한다 |
| `main()` → `solution()` | `printf` → `return` |
| 로직 함수 | 무변경. 전역 배열 + DFS 백트래킹 그대로다 |

## 이 문제만의 포인트

- **결정할 것은 "번들을 살까 말까" 하나뿐이다.** 번들은 최대 15개라 경우의 수가 2^15 = 32,768가지다.
  삼성 기출의 연산자 배치·조삼모사처럼 "산다 / 안 산다" 두 갈래로 나누는 DFS로 전부 해 본다.
- **힌트권은 있는 만큼 다 쓴다.** `cost[i][j] > cost[i][j+1]` 이라 한 장이라도 더 쓰면 항상 싸고,
  i번 힌트권은 i번 스테이지에서만 쓸 수 있어 남겨 봐야 쓸 데가 없다. 그래서 `min(가진 장수, n - 1)`장을 쓴다.
- **순서가 꼬이지 않는다.** i번 스테이지 번들에는 i+1번 이상의 힌트권만 있으므로,
  사는 즉시 `ticket[]`에 더해 두면 그 스테이지에 도착했을 때 이미 들어와 있다.
- DFS에서 번들을 산 뒤 **돌아올 때 힌트권을 되돌리는 것**(`ticket[...]--`)을 잊으면 안 된다. 백트래킹의 기본이다.
- 비용이 모두 양수라 `sum >= answer` 이면 가지치기할 수 있다.

## 검증에 쓴 방법

- 문제의 입출력 예 6개를 전부 통과했다 (제출형을 채점기처럼 따로 링크해서도 확인했다).
- DFS와 다른 방식인 **비트마스크 전수 조사**(2^(n-1)개 부분집합을 전부 나열) 구현과 무작위 300개를 비교해 전부 일치했다.
- 최대 크기(n = 16, k = 19)에서 74ms.

## 로컬 테스트

```bash
cd programmers/2025카카오2차_힌트스테이지
g++ -O2 -o run local_test.cpp                # solution.cpp 를 include 해서 빌드
g++ -O2 -DREPEAT_TEST -o rep local_test.cpp  # 재호출 검사
g++ -O2 -o orig ../../swtest/프로그래머스_2025_하반기2차_힌트스테이지.cpp
./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt
```

> 제출할 때는 `solution.cpp` 를 통째로 붙여 넣는다. `local_test.cpp` 는 제출하지 않는다.
