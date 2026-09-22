# [프로그래머스] 2025 카카오 하반기 2차 - 기차 선로 (Lv. 3)

`swtest/` 의 표준입출력형 풀이를 프로그래머스 제출형으로 옮긴 것이다.
**코드는 swtest 판과 같다.** `input()`이 인자를 받고 `main()`이 `solution()`이 된 것만 다르다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/프로그래머스_2025_하반기2차_기차선로.cpp](../../swtest/프로그래머스_2025_하반기2차_기차선로.cpp) |
| 문제 | https://school.programmers.co.kr/learn/courses/30/lessons/468381 |
| 반환 타입 | `int` |
| 제출 파일 | `solution.cpp` (통째로 붙여 넣기) |
| 로컬 테스트 | `local_test.cpp` (제출하지 않음) |

## 시그니처

```cpp
int solution(int** grid, size_t grid_rows, size_t grid_cols)
```

## 원본 stdin을 어떻게 인자로 바꿨나

`local_test.cpp` 가 원본과 똑같은 형식으로 읽어 `solution()`에 넘긴다.
즉 아래 읽기 순서가 곧 인자 순서다.

```cpp

```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | `#include <vector>` 추가, `using namespace std;` (프로그래머스 템플릿과 같은 형태) |
| `input()` | `scanf`를 인자 대입으로 교체. 전역 초기화 루프는 원본 그대로 |
| `main()` → `solution()` | `T` 루프 껍데기 제거, `printf` → `return` |
| 로직 함수 | 무변경. 전역 배열을 쓰는 C 스타일 그대로다 |

## 원본과 달라진 곳

**버그 수정** (swtest 원본은 기록용이라 그대로 두었다)

- `solution.cpp:91` : int dc[] = { 0,1,0,-1 };   // [버그수정] 원본은 쉼표가 빠져 원소가 3개였다

## 검증

랜덤 입력을 만들어 원본 실행 파일과 `cmp` 로 대조했고, 전부 일치했다.
`-DREPEAT_TEST` 로 빌드해 같은 인자로 두 번 호출해도 결과가 같은 것(재호출 안전)까지 확인했다.

## 로컬 테스트

```bash
cd programmers/2025카카오2차_기차선로
g++ -O2 -o run local_test.cpp                # solution.cpp 를 include 해서 빌드
g++ -O2 -DREPEAT_TEST -o rep local_test.cpp  # 재호출 검사
g++ -O2 -o orig ../../swtest/프로그래머스_2025_하반기2차_기차선로.cpp
./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt
```

> 제출할 때는 `solution.cpp` 를 통째로 붙여 넣는다. `local_test.cpp` 는 제출하지 않는다.
