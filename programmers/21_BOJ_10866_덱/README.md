# 21 덱 (BOJ 10866) — 프로그래머스 함수형

덱을 배열 하나와 포인터 둘(`front`, `back`)로 구현하고 8가지 명령을 처리한다.

- 원본 : [swtest/21_BOJ_10866_덱.cpp](../../swtest/21_BOJ_10866_덱.cpp)
- 복습 노트 : [docs/review/G02/21_BOJ_10866_덱.md](../../docs/review/G02/21_BOJ_10866_덱.md)
- 원문제 : https://www.acmicpc.net/problem/10866

## 시그니처

```cpp
std::vector<int> solution(std::vector<std::string> commands)
```

## stdin → 인자 대응

| 원본 stdin | 원본 코드 | solution |
|---|---|---|
| 1행 `N` | `scanf("%d", &N)` | **인자 아님** — `commands.size()`로 대신한다 |
| N줄 명령 | `scanf("%s", command)` + push면 `scanf("%d", &value)` | `commands[i]` — `"push_back 10"` / `"pop_front"` 처럼 **한 줄 통째로** |

반환 → 원본 출력 : 조회 명령(`pop_front` `pop_back` `size` `empty` `front` `back`)마다 한 줄씩 찍던 정수들을 순서대로 담은 배열.
`push_front` / `push_back`은 출력이 없으므로 배열에도 들어가지 않는다.

## 이 문제의 변환 포인트 — `input()`이 없는 유형

대부분의 풀이는 `void input()`이 따로 있지만, 이 파일은 **`scanf`가 `main` 안에서 로직과 섞여 있다**
(16·17·18·19·20·21번과 드래곤커브가 같은 구조다).

이럴 때 명령 파싱을 STL로 "개선"하면 분기 로직이 통째로 다른 코드가 된다.
여기서는 **`scanf` → `sscanf` 치환만** 했다. 읽는 대상이 stdin에서 문자열로 바뀔 뿐이라
원본의 `strcmp` 사슬은 한 글자도 건드리지 않는다.

```cpp
sscanf(commands[i].c_str(), "%s", command);          // 명령 이름
sscanf(commands[i].c_str(), "%*s %d", &value);       // push 계열의 인자 (%*s 로 앞 토큰 버림)
```

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | `#include <vector>`, `#include <string>` 추가 |
| `main()` → `solution()` | `scanf("%d", &N)` → `commands.size()`, 각 `scanf` → `sscanf` |
| 출력 | `printf("%d\n", ...)` 9곳 → `answer.push_back(...)` |
| 반환 | `return 0;` → `return answer;` |
| 로직 | **무변경** (`strcmp` 구현, `front`/`back` 조작, 8개 분기 전부 그대로) |

## 재호출 주의점

`front = back = OFFSET;` 이 원본에서도 `main` 첫 줄에 있었고, 그대로 `solution()` 첫 줄로 옮겨 왔다.
이 한 줄이 곧 재호출 초기화 역할을 한다 — 덱의 내용(`deque[]`)은 `[front, back)` 구간만 읽으므로
이전 호출의 잔상이 남아 있어도 결과에 영향이 없다.

> `-DREPEAT_TEST` 로 300케이스 검사 통과.

## ⚠️ 원본에 남아 있는 주의사항 (고치지 않음)

원본 주석이 이미 지적하고 있는 내용이라 그대로 옮겨 왔다.

- `OFFSET`이 `MAX/2`(= 5250)인데 N은 최대 10,000이다. `push_front`만 계속 들어오면 `front`가 0 아래로 내려가 배열 밖을 건드린다. `OFFSET`을 `MAX`로 두면 안전하다.
- 전역 `strcmp`가 표준 `strcmp`와 이름이 같다. 여기서는 `<vector>` / `<string>`을 추가해도 컴파일이 통과하는 것을 확인했지만, `<cstring>`을 직접 넣으면 깨진다.

## `using namespace std;`를 쓰지 않는 이유 — 이 파일이 대표 사례

이 파일에는 전역 `int deque[MAX * 2];` 가 있다. `using namespace std;`를 넣으면 `std::deque`와 겹친다.
`programmers/` 전체가 같은 이유로 `std::vector` / `std::string`을 명시한다.

## 로컬 테스트

```bash
cd programmers/21_BOJ_10866_덱
g++ -O2 -DLOCAL_TEST -o run solution.cpp
printf '8\npush_front 1\npush_back 2\nfront\nback\nsize\nempty\npop_front\npop_back\n' | ./run
# 1
# 2
# 2
# 0
# 1
# 2
```

원본과 대조:

```bash
g++ -O2 -o orig ../../swtest/21_BOJ_10866_덱.cpp
printf '8\npush_front 1\npush_back 2\nfront\nback\nsize\nempty\npop_front\npop_back\n' > in.txt
./orig < in.txt > a.out; ./run < in.txt > b.out; cmp a.out b.out
```

> 제출할 때는 `solution.cpp` 맨 아래 `#ifdef LOCAL_TEST` 블록을 빼고 복사한다.
