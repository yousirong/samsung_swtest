# [코드트리] 2020 하반기 오후 1번 - 청소는 즐거워

`swtest/` 의 표준입출력형 풀이를 프로그래머스 함수형으로 옮긴 것이다.
로직 함수는 건드리지 않았고 입력(`scanf`)과 출력(`printf`) 껍데기만 바꿨다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/코드트리_2020_하반기오후1번_청소는즐거워.cpp](../../swtest/코드트리_2020_하반기오후1번_청소는즐거워.cpp) |
| 문제 | https://www.acmicpc.net/problem/20057) |
| 반환 타입 | `int` |
| 원본 버그 | 없음 |

## 시그니처

```cpp
int solution(vector<vector<int>> board)
```

## 원본 stdin을 어떻게 인자로 바꿨나

하네스(`#ifdef LOCAL_TEST`)가 원본과 똑같은 형식으로 읽어 `solution()`에 넘긴다.
즉 아래 읽기 순서가 곧 인자 순서다.

```cpp
scanf("%d", &n);                 // 원본 scanf 순서 그대로
scanf("%d", &board[r][c]);
```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | `#include <vector>` 추가. `using namespace std;` (프로그래머스 템플릿과 같은 형태) |
| `input()` | `scanf`를 인자 대입으로 교체. 전역 초기화 루프는 원본 그대로 |
| `main()` → `solution()` | `T` 루프 껍데기 제거, `printf` → `return` |
| 로직 함수 | 무변경 |

## 검증

랜덤 입력을 만들어 원본 실행 파일과 `cmp` 로 대조했고, 전부 일치했다.
`-DREPEAT_TEST` 로 빌드해 같은 인자로 두 번 호출해도 결과가 같은 것(재호출 안전)까지 확인했다.

## 로컬 테스트

```bash
cd programmers/2020하오후1_청소는즐거워
g++ -O2 -DLOCAL_TEST -o run solution.cpp        # 하네스 포함 빌드
g++ -O2 -DLOCAL_TEST -DREPEAT_TEST -o rep solution.cpp   # 재호출 검사
g++ -O2 -o orig ../../swtest/코드트리_2020_하반기오후1번_청소는즐거워.cpp
./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt
```

> 제출할 때는 `solution.cpp` 맨 아래 `#ifdef LOCAL_TEST` 블록을 빼고 복사한다.
