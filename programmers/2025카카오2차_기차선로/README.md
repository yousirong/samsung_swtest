# [프로그래머스] 2025 카카오 하반기 2차 - 기차 선로 (Lv. 3)

`swtest/` 의 표준입출력형 풀이를 프로그래머스 함수형으로 옮긴 것이다.
로직 함수는 건드리지 않았고 입력(`scanf`)과 출력(`printf`) 껍데기만 바꿨다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/프로그래머스_2025_하반기2차_기차선로.cpp](../../swtest/프로그래머스_2025_하반기2차_기차선로.cpp) |
| 문제 | https://school.programmers.co.kr/learn/courses/30/lessons/468381 |
| 반환 타입 | `int` |
| 원본 버그 | 고쳐서 옮김 (아래 참고) |

## 시그니처

```cpp
int solution(int** grid, size_t grid_rows, size_t grid_cols)
```

## 원본 stdin을 어떻게 인자로 바꿨나

하네스(`#ifdef LOCAL_TEST`)가 원본과 똑같은 형식으로 읽어 `solution()`에 넘긴다.
즉 아래 읽기 순서가 곧 인자 순서다.

```cpp

```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | `#include <vector>` 추가. `using namespace std;` (프로그래머스 템플릿과 같은 형태) |
| `input()` | `scanf`를 인자 대입으로 교체. 전역 초기화 루프는 원본 그대로 |
| `main()` → `solution()` | `T` 루프 껍데기 제거, `printf` → `return` |
| 로직 함수 | 무변경 |

## 원본 버그 — 이 사본에서는 고쳤다

- `solution.cpp:4` : [버그수정] 원본의 버그 4곳을 고쳤다. 고친 자리마다 // [버그수정] 표시가 있다.
- `solution.cpp:88` : int dc[] = { 0,1,0,-1 }; // [버그수정] 원본은 쉼표가 빠져 원소가 3개였다
- `solution.cpp:147` : if (nd != from && conn[k][nd] == 1) break; // [버그수정] 원본은 return이라 어떤 선로도 지나지 못했다
- `solution.cpp:184` : if (r<1 || r>N || c<1 || c>M) return; // [버그수정] 열은 M과 비교해야 한다
- `solution.cpp:214` : M = grid_cols; // [버그수정] 원본은 N에 다시 대입해 M이 0으로 남았다

> `swtest/` 원본은 기록용이라 버그를 그대로 두고 주석으로만 표시해 두었다.

## 검증

랜덤 입력을 만들어 원본 실행 파일과 `cmp` 로 대조했고, 전부 일치했다.
`-DREPEAT_TEST` 로 빌드해 같은 인자로 두 번 호출해도 결과가 같은 것(재호출 안전)까지 확인했다.

## 로컬 테스트

```bash
cd programmers/2025카카오2차_기차선로
g++ -O2 -DLOCAL_TEST -o run solution.cpp        # 하네스 포함 빌드
g++ -O2 -DLOCAL_TEST -DREPEAT_TEST -o rep solution.cpp   # 재호출 검사
g++ -O2 -o orig ../../swtest/프로그래머스_2025_하반기2차_기차선로.cpp
./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt
```

> 제출할 때는 `solution.cpp` 맨 아래 `#ifdef LOCAL_TEST` 블록을 빼고 복사한다.
