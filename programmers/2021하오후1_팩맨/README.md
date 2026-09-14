# 2021 하반기 오후 1번 — 팩맨 (프로그래머스 함수형)

4×4 격자에서 몬스터가 복제·이동하고 팩맨이 3칸 움직이며 잡아먹는다. TURN턴 뒤 남은 몬스터 수를 구한다.

- 원본 : [swtest/코드트리_2021_하반기오후1번_팩맨.cpp](../../swtest/코드트리_2021_하반기오후1번_팩맨.cpp)
- 복습 노트 : 없음 (docs/INDEX.md 미등록)
- 원문제 : [코드트리 팩맨](https://www.codetree.ai/training-field/frequent-problems/problems/pacman) (백준 23290 "마법사 상어와 복제"와 같은 문제)

## 시그니처

```cpp
int solution(int t, int r, int c, std::vector<std::vector<int>> monsters)
```

## stdin → 인자 대응

| 원본 stdin | 원본 코드 | solution |
|---|---|---|
| 1행 `M TURN r c` | `scanf("%d %d %D %d", &M, &TURN, &packMan.r, &packMan.c)` | `M`은 **인자 아님** — `monsters.size()`로 대신한다<br>`TURN`→`t`, `packMan.r`→`r`, `packMan.c`→`c` |
| 이어서 M줄 `r c d` | `scanf("%d %d %d", &r, &c, &d)` | `monsters[i] = {r, c, d}` |

반환 → 원본 출력 : `printf("%d\n", getAnswer())` 의 정수 하나.

> 인자 순서는 **원본 scanf 순서 그대로**다. `M`만 배열 길이로 대체되어 빠졌다.

## 바뀐 지점

| 위치 | 변경 |
|---|---|
| 헤더 | `#include <vector>` 추가. **`using namespace std;`는 쓰지 않는다** (아래 참고) |
| `input()` | `input(int t, int pr, int pc, const std::vector<std::vector<int>>&)` — `scanf` 2곳을 인자 대입으로. 전역 초기화 루프는 원본 그대로 |
| `main()` → `solution()` | `T = 1; for (tc...)` 껍데기 제거, `printf` → `return` |
| `solution()` 첫 줄 | `pcnt = 0;` 추가 |
| 로직 함수 | **무변경** (`moveMonster` `getMonster` `movePackMan` `disappear` `simulate` `getAnswer`) |

### `using namespace std;`를 쓰지 않는 이유

이 파일 하나만 보면 써도 문제가 없다. 하지만 `programmers/` 전체에서 규칙을 하나로 맞췄다.
다른 문제에는 `int move[]`, `int next[][]`, `int count;`, `int deque[]` 같은 전역이 있어
`using namespace std;`를 넣으면 `std::move` 등과 이름이 겹쳐 **컴파일이 깨진다**.

```
error: reference to 'move' is ambiguous
```

그래서 63개 전부 `std::vector`를 명시하는 방식으로 통일했다.

## 재호출 주의점

`position[64][3]`과 `pcnt`는 원본에서 `main()` 진입 직후 `DFS(0)`로 **딱 한 번** 만들어졌다.
함수형은 채점기가 `solution()`을 여러 번 부를 수 있으므로, `pcnt`를 0으로 되돌리지 않으면
두 번째 호출에서 `position`에 64개가 더 쌓여 배열을 넘긴다.

```cpp
pcnt = 0;    // [추가] 재호출 대비
DFS(0);
```

나머지 전역(`monster` `deadBody` `grid` `packMan` `M` `TURN`)은 `input()`이 매번 전 범위를 덮어써서 잔상이 없다.

> 하네스를 `-DREPEAT_TEST`로 빌드하면 같은 인자로 두 번 호출해 결과가 같은지 확인한다. 300케이스 통과.

## ⚠️ 원본 버그 3개 — 수정해서 옮겼다

원본 `swtest/코드트리_2021_하반기오후1번_팩맨.cpp`는 **입력에 따라 세그폴트한다**.
랜덤 입력 200개 중 46개에서 원본과 결과가 갈렸고, 상당수는 크래시였다.
아래 3곳을 고쳐야 정상 동작하며, solution.cpp에는 `// [버그수정]` 표시를 달아 반영했다.

| # | 위치 | 원본 | 고친 것 | 증상 |
|---|---|---|---|---|
| 1 | `input()` 벽 세우기 | `for (int r = 1; r <= 5; r++)` | `for (int r = 0; r <= 5; r++)` | 0행이 벽으로 안 막혀 팩맨·몬스터가 격자 위로 빠져나가 배열 밖을 건드린다 → **세그폴트** |
| 2 | `moveMonster()` | `if ((nr == packMan.r && packMan.c)` | `if ((nr == packMan.r && nc == packMan.c)` | `packMan.c`는 1~4라 항상 참. **행만 같으면** 팩맨이 있는 것으로 오판해 몬스터가 엉뚱하게 못 움직인다 |
| 3 | `simulate()` | `step = maxMonster = 1;` | `step = maxMonster = -1;` | 갱신 조건이 `tmp > maxMonster`라, 0마리·1마리만 먹는 경로를 **절대 고르지 못한다** |

세 곳을 고친 뒤 300케이스에서 크래시 0건, 재호출 불일치 0건이다.

> `swtest/` 원본은 아직 고치지 않았다. 원본에도 반영하려면 같은 3줄을 바꾸면 된다.

### 덤으로 사라진 것

원본 `scanf("%d %d %D %d", ...)` 의 대문자 `%D`는 오타다.
MinGW에서는 `%d`처럼 동작해 넘어가지만 리눅스 glibc에서는 파싱이 그 자리에서 멈춘다.
함수형으로 바꾸면서 `scanf` 자체가 사라져 **자연히 해소**됐다.

## 로컬 테스트

```bash
cd programmers/2021하오후1_팩맨
g++ -O2 -DLOCAL_TEST -o run solution.cpp
printf '3 1 1 1\n2 2 1\n3 3 5\n4 4 3\n' | ./run
```

원본과 대조하려면 (원본에 위 3개 수정을 먼저 적용해야 의미가 있다):

```bash
g++ -O2 -o orig ../../swtest/코드트리_2021_하반기오후1번_팩맨.cpp
printf '3 1 1 1\n2 2 1\n3 3 5\n4 4 3\n' | ./orig > a.out
printf '3 1 1 1\n2 2 1\n3 3 5\n4 4 3\n' | ./run  > b.out
cmp a.out b.out
```

재호출 안전성 검사:

```bash
g++ -O2 -DLOCAL_TEST -DREPEAT_TEST -o rep solution.cpp
printf '3 1 1 1\n2 2 1\n3 3 5\n4 4 3\n' | ./rep    # "NOT RE-ENTRANT"가 안 나오면 통과
```

> 제출할 때는 `solution.cpp` 맨 아래 `#ifdef LOCAL_TEST` 블록을 빼고 복사한다.
