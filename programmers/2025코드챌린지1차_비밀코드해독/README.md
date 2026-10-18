# [프로그래머스] 2025 코드챌린지 1차 예선 - 비밀 코드 해독

`swtest/` 의 표준입출력형 풀이를 프로그래머스 제출형으로 옮긴 것이다.
**코드는 swtest 판과 같다.** `input()`이 인자를 받고 `main()`이 `solution()`이 된 것만 다르다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/프로그래머스_2025_코드챌린지1차예선_비밀코드해독.cpp](../../swtest/프로그래머스_2025_코드챌린지1차예선_비밀코드해독.cpp) |
| 문제 | https://school.programmers.co.kr/learn/courses/30/lessons/388352 |
| 반환 타입 | `int` |
| 제출 파일 | `solution.cpp` (통째로 붙여 넣기) |
| 로컬 테스트 | `local_test.cpp` (제출하지 않음) |

## 시그니처 (템플릿 그대로)

```cpp
int solution(int n, vector<vector<int>> q, vector<int> ans) {
    int answer = 0;
    input(n, q, ans);     // 추가
    DFS(0, 1);            // 추가
    answer = COUNT;       // 추가
    return answer;
}
```

전역은 `N`, `M`, `QUERY`, `RESP`, `COUNT`처럼 대문자라 템플릿의 `n`, `q`, `ans`, `answer`와 겹치지 않는다.

## 원본 stdin을 어떻게 인자로 바꿨나

첫 줄 `n m`, 시도 m줄(5개씩), 응답 m개.

```cpp
scanf("%d %d", &n, &m);   // 원본 scanf 순서 그대로
scanf("%d", &q[i][j]);    // m줄 x 5개
scanf("%d", &ans[i]);     // m개
```

## 이 문제만의 포인트

- **1 ~ n 중 5개 고르기를 DFS로 전부 만든다.** C(30, 5) = 142,506가지뿐이다.
- **`DFS(L, start)`에서 다음 수는 `i + 1`부터 고른다.** 그래서 오름차순 조합만 나와 같은 조합을 두 번 세지 않는다.
- **고른 수를 `selected[x] = 1`로 표시**하면 시도의 5개가 비밀 코드에 있는지 바로 안다. 겹친 개수가 응답과 다르면 탈락.
- 고를 때 1, 돌아올 때 0 (원상복구).

## 검증에 쓴 방법

- 입출력 예 2개(3, 5) 통과. `-DREPEAT_TEST` 재호출 검사도 통과.
- n = 30, 시도 `[1,2,3,4,5]` 응답 0 → 6 ~ 30 중 5개 = C(25, 5) = 53,130 이 나오는 것을 확인.

## 로컬 테스트

```bash
cd programmers/2025코드챌린지1차_비밀코드해독
g++ -O2 -o run local_test.cpp                # solution.cpp 를 include 해서 빌드
g++ -O2 -DREPEAT_TEST -o rep local_test.cpp  # 재호출 검사
printf "10 5\n1 2 3 4 5\n6 7 8 9 10\n3 7 8 9 10\n2 5 7 9 10\n3 4 5 6 7\n2 3 4 3 3\n" | ./run   # 3
```

> 제출할 때는 `solution.cpp` 를 통째로 붙여 넣는다. `local_test.cpp` 는 제출하지 않는다.
