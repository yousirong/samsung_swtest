# [코드트리] 2025 상반기 오전 1번 - 민트 초코 우유

`swtest/` 의 표준입출력형 풀이를 프로그래머스 제출형으로 옮긴 것이다.
**코드는 swtest 판과 같다.** `input()`이 인자를 받고 `main()`이 `solution()`이 된 것만 다르다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/코드트리_2025_상반기오전1번_민트초코우유.cpp](../../swtest/코드트리_2025_상반기오전1번_민트초코우유.cpp) |
| 문제 | https://www.codetree.ai/training-field/frequent-problems/problems/mint-choco-milk |
| 반환 타입 | `vector<vector<int>>` |
| 제출 파일 | `solution.cpp` (통째로 붙여 넣기) |
| 로컬 테스트 | `local_test.cpp` (제출하지 않음) |

## 시그니처

```cpp
vector<vector<int>> solution(vector<string> foods, vector<vector<int>> believes, int t)
```

## 원본 stdin을 어떻게 인자로 바꿨나

`local_test.cpp` 가 원본과 똑같은 형식으로 읽어 `solution()`에 넘긴다.
즉 아래 읽기 순서가 곧 인자 순서다.

```cpp
scanf("%d %d", &n, &t);   // 원본 scanf 순서 그대로
scanf(" %c", &foods[r][c]);
scanf("%d", &believes[r][c]);
```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | `#include <vector>` 추가, `using namespace std;` (프로그래머스 템플릿과 같은 형태) |
| `input()` | `scanf`를 인자 대입으로 교체. 전역 초기화 루프는 원본 그대로 |
| `main()` → `solution()` | `T` 루프 껍데기 제거, `printf` → 결과 배열에 담았다가 `return` |
| 로직 함수 | 무변경. 전역 배열을 쓰는 C 스타일 그대로다 |

## 원본과 달라진 곳

**이름 변경**

- `solution.cpp:96` : 전역 `index` → `groupCount` (리눅스 `<strings.h>` 의 `index()` 함수와 겹칠 수 있다. Windows MinGW에서는 안 걸려도 채점 서버에서 걸릴 수 있다)

**인자 형태** — 음식 격자는 `T`/`C`/`M` 문자라서 `vector<string> foods` 로 받는다 (`#include <string>` 추가). 신앙심 격자는 `vector<vector<int>> believes`.

**반환 형식** — 원본은 하루마다 신봉 음식 7종류의 신앙심 합을 `민트초코우유, 민트초코, 민트우유, 초코우유, 우유, 초코, 민트` 순서로 한 줄에 출력했다.
하루를 `vector<int>`(길이 7) 하나로, 전체를 `vector<vector<int>>`(길이 T)로 반환한다.

## 검증

랜덤 입력을 만들어 원본 실행 파일과 `cmp` 로 대조했고, 전부 일치했다.
`-DREPEAT_TEST` 로 빌드해 같은 인자로 두 번 호출해도 결과가 같은 것(재호출 안전)까지 확인했다.

## 로컬 테스트

```bash
cd programmers/2025상오전1_민트초코우유
g++ -O2 -o run local_test.cpp                # solution.cpp 를 include 해서 빌드
g++ -O2 -DREPEAT_TEST -o rep local_test.cpp  # 재호출 검사
g++ -O2 -o orig ../../swtest/코드트리_2025_상반기오전1번_민트초코우유.cpp
./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt
```

> 제출할 때는 `solution.cpp` 를 통째로 붙여 넣는다. `local_test.cpp` 는 제출하지 않는다.
