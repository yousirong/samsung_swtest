# [프로그래머스] 2025 코드챌린지 2차 예선 - 서버 증설 횟수

`swtest/` 의 표준입출력형 풀이를 프로그래머스 제출형으로 옮긴 것이다.
**코드는 swtest 판과 같다.** `input()`이 인자를 받고 `main()`이 `solution()`이 된 것만 다르다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/프로그래머스_2025_코드챌린지2차예선_서버증설횟수.cpp](../../swtest/프로그래머스_2025_코드챌린지2차예선_서버증설횟수.cpp) |
| 문제 | https://school.programmers.co.kr/learn/courses/30/lessons/389479 |
| 반환 타입 | `int` |
| 제출 파일 | `solution.cpp` (통째로 붙여 넣기) |
| 로컬 테스트 | `local_test.cpp` (제출하지 않음) |

## 시그니처

```cpp
int solution(vector<int> players, int m, int k)
```

## 원본 stdin을 어떻게 인자로 바꿨나

이용자 수 24개, 그다음 `m k`.

```cpp
scanf("%d", &p[i]);   // 원본 scanf 순서 그대로 (24개)
scanf("%d %d", &m, &k);
```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | 프로그래머스 템플릿대로 `#include <string>`, `#include <vector>`, `using namespace std;` |
| `input()` | `scanf`를 인자 대입으로 교체. `expire[]` 초기화는 원본 그대로 |
| `main()` → `solution()` | `printf` → `return` |
| 로직 함수 | 무변경 |

> `solution()`의 매개변수 `players`가 전역 배열 `players`와 이름이 같다.
> `solution()` 안에서는 매개변수가 전역을 가리지만, 그 안에서는 `input()`에 넘기기만 하므로 문제없다.

## 이 문제만의 포인트

- **필요한 서버 수 = 이용자 / m (내림).**
- **0시부터 차례로, 모자랄 때 모자란 만큼만 늘린다.** 미리 늘리면 반납도 빨라질 뿐이라 이득이 없다.
- **반납은 `expire[i + k]` 배열에 적어 둔다.** 그 시각이 되면 `running`에서 뺀다. 큐가 필요 없다.
- 반납 시각은 `i + k`다. 표에서 k = 5, 2시에 늘린 서버는 2 ~ 6시 칸까지 돌고 7시 칸에서 빠진다.

## 검증에 쓴 방법

- 입출력 예 3개(7, 11, 12) 통과. 같은 인자로 두 번 불러도 결과가 같은 것(`-DREPEAT_TEST`)도 확인했다.

## 로컬 테스트

```bash
cd programmers/2025코드챌린지2차_서버증설횟수
g++ -O2 -o run local_test.cpp                # solution.cpp 를 include 해서 빌드
g++ -O2 -DREPEAT_TEST -o rep local_test.cpp  # 재호출 검사
echo "0 2 3 3 1 2 0 0 0 0 4 2 0 6 0 4 2 13 3 5 10 0 1 5 3 5" | ./run   # 7
```

> 제출할 때는 `solution.cpp` 를 통째로 붙여 넣는다. `local_test.cpp` 는 제출하지 않는다.
