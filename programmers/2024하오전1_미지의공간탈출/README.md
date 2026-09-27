# [코드트리] 2024 하반기 오전 1번 - 미지의 공간 탈출

`swtest/` 의 표준입출력형 풀이를 프로그래머스 제출형으로 옮긴 것이다.
**코드는 swtest 판과 같다.** `input()`이 인자를 받고 `main()`이 `solution()`이 된 것만 다르다.

| 항목 | 내용 |
|---|---|
| 원본 | [swtest/코드트리_2024_하반기오전1번_미지의공간탈출.cpp](../../swtest/코드트리_2024_하반기오전1번_미지의공간탈출.cpp) |
| 문제 | https://www.codetree.ai/training-field/frequent-problems/problems/escape-from-the-unknown-space |
| 반환 타입 | `int` |
| 제출 파일 | `solution.cpp` (통째로 붙여 넣기) |
| 로컬 테스트 | `local_test.cpp` (제출하지 않음) |

## 시그니처

```cpp
int solution(vector<vector<int>> ground, vector<vector<vector<int>>> faces, vector<vector<int>> anomalies)
```

## 원본 stdin을 어떻게 인자로 바꿨나

`local_test.cpp` 가 원본과 똑같은 형식으로 읽어 `solution()`에 넘긴다.
즉 아래 읽기 순서가 곧 인자 순서다.

```cpp
scanf("%d %d %d", &n, &m, &f);   // 원본 scanf 순서 그대로
scanf("%d", &ground[r][c]);
scanf("%d", &faces[i][r][c]);
scanf("%d %d %d %d", &anomalies[i][0], &anomalies[i][1], &anomalies[i][2], &anomalies[i][3]);
```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | `#include <vector>` 추가, `using namespace std;` (프로그래머스 템플릿과 같은 형태) |
| `input()` | `scanf`를 인자 대입으로 교체. 전역 초기화 루프는 원본 그대로 |
| `main()` → `solution()` | `T` 루프 껍데기 제거, `printf` → `return` |
| 로직 함수 | 무변경. 전역 배열을 쓰는 C 스타일 그대로다 |

## 원본과 달라진 곳

**이름 변경** (`using namespace std;` 와 충돌)

- `solution.cpp:97` : // [이름변경] end -> endPos (std::end 와 겹친다), next -> nextCell (std::next 와 겹친다)

## 검증

랜덤 입력을 만들어 원본 실행 파일과 `cmp` 로 대조했고, 전부 일치했다.
`-DREPEAT_TEST` 로 빌드해 같은 인자로 두 번 호출해도 결과가 같은 것(재호출 안전)까지 확인했다.

## 로컬 테스트

```bash
cd programmers/2024하오전1_미지의공간탈출
g++ -O2 -o run local_test.cpp                # solution.cpp 를 include 해서 빌드
g++ -O2 -DREPEAT_TEST -o rep local_test.cpp  # 재호출 검사
g++ -O2 -o orig ../../swtest/코드트리_2024_하반기오전1번_미지의공간탈출.cpp
./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt
```

> 제출할 때는 `solution.cpp` 를 통째로 붙여 넣는다. `local_test.cpp` 는 제출하지 않는다.
