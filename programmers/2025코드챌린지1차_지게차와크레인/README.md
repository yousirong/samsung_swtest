# [프로그래머스] 2025 코드챌린지 1차 예선 - 지게차와 크레인

`swtest/` 의 표준입출력형 풀이를 프로그래머스 제출형으로 옮긴 것이다.
**코드는 swtest 판과 같다.** `input()`이 인자를 받고 `main()`이 `solution()`이 된 것만 다르다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/프로그래머스_2025_코드챌린지1차예선_지게차와크레인.cpp](../../swtest/프로그래머스_2025_코드챌린지1차예선_지게차와크레인.cpp) |
| 문제 | https://school.programmers.co.kr/learn/courses/30/lessons/388353 |
| 반환 타입 | `int` |
| 제출 파일 | `solution.cpp` (통째로 붙여 넣기) |
| 로컬 테스트 | `local_test.cpp` (제출하지 않음) |

## 시그니처 (템플릿 그대로)

```cpp
int solution(vector<string> storage, vector<string> requests) {
    int answer = 0;
    input(storage, requests);     // 추가
    answer = simulate();          // 추가
    return answer;
}
```

템플릿 줄은 그대로 두고 가운데 두 줄만 넣었다. 전역 변수는 `MAP`, `N`, `REQ`처럼 대문자라
템플릿의 `storage`, `requests`, `answer`와 이름이 겹치지 않는다.

## 원본 stdin을 어떻게 인자로 바꿨나

첫 줄 `n m`, 창고 n줄, 요청 수 q, 요청 q줄.

```cpp
scanf("%d %d", &n, &m);   // 원본 scanf 순서 그대로
scanf("%s", line);        // 창고 한 줄 (n번)
scanf("%d", &q);
scanf("%s", req);         // 요청 하나 (q번)
```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | 프로그래머스 템플릿대로 `#include <string>`, `#include <vector>`, `using namespace std;` |
| `input()` | `scanf`를 인자 대입으로 교체. 요청은 `REQ[i][0]`(종류), `REQ[i][1]`(두 글자면 같은 문자, 한 글자면 0)으로 옮긴다 |
| `main()` → `solution()` | 템플릿을 그대로 두고 `input()`, `simulate()` 두 줄만 넣었다 |
| 로직 함수 | 무변경 |

## 이 문제만의 포인트

- **테두리를 빈 칸으로 한 줄 두른다.** 그러면 "창고 바깥" = (0, 0)에서 빈 칸만 밟고 갈 수 있는 곳이다.
- **지게차 = 바깥에서 BFS.** 빈 칸만 따라 퍼지다가, 이웃이 요청한 종류면 `pick[][]`에 표시만 한다.
- **BFS가 끝난 뒤 한꺼번에 비운다.** 도중에 비우면 BFS가 그 안으로 들어가, 이번 요청에는 닿을 수 없던 안쪽 컨테이너까지 꺼낸다.
- **크레인 = 전체를 훑어 전부 비운다.**
- 빈 칸이라고 다 바깥이 아니다. 크레인이 안쪽에 만든 빈 칸은 바깥과 이어지기 전까지 BFS가 닿지 않는다.
- `remove`는 `<stdio.h>`의 함수 이름이라 꺼낼 칸 표시 배열은 `pick`으로 지었다.

## 검증에 쓴 방법

- 입출력 예 2개(11, 4) 통과. `-DREPEAT_TEST` 재호출 검사도 통과.
- BFS 대신 "바깥과 이어진 빈 칸"을 반복 갱신으로 구하는 별도 구현과 무작위 3,000개를 비교해 전부 일치했다.

## 로컬 테스트

```bash
cd programmers/2025코드챌린지1차_지게차와크레인
g++ -O2 -o run local_test.cpp                # solution.cpp 를 include 해서 빌드
g++ -O2 -DREPEAT_TEST -o rep local_test.cpp  # 재호출 검사
printf "4 5\nAZWQY\nCAABX\nBBDDA\nACACA\n3\nA\nBB\nA\n" | ./run   # 11
```

> 제출할 때는 `solution.cpp` 를 통째로 붙여 넣는다. `local_test.cpp` 는 제출하지 않는다.
