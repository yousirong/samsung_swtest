# vector 정리 — C 스타일로 풀던 사람이 프로그래머스에 제출할 때 필요한 만큼만

`swtest/`처럼 전역 고정 배열로 푸는 사람이, 프로그래머스 C++ 제출 때문에 어쩔 수 없이 만나는
`vector` 만 추려 정리한다. **STL을 새로 배우자는 게 아니라, 껍데기를 통과하는 데 필요한 것만** 본다.

이 폴더의 모든 풀이는 아래 한 문장을 지킨다.

> `vector`는 **함수 경계에서만** 쓴다. 안쪽은 전부 지금까지 쓰던 전역 배열 그대로다.

---

## 1. 한 줄 요약

**`vector<int>` = 길이를 스스로 들고 다니는 `int` 배열.**

```cpp
int   a[100];   int acnt;    // 내가 길이를 따로 관리
vector<int> v;               // v가 길이를 스스로 관리 (v.size())
```

그게 거의 전부다. 읽고 쓰는 문법(`v[i]`)은 배열과 똑같다.

| 내가 쓰던 것 | vector |
|---|---|
| `int a[100];` | `vector<int> v(100);` |
| `int a[100] = {0};` | `vector<int> v(100, 0);` |
| `int a[3][4];` | `vector<vector<int>> v(3, vector<int>(4));` |
| `acnt` (내가 센 개수) | `v.size()` |
| `a[i]` | `v[i]` — 똑같다 |
| `acnt = 0;` (비우기) | `v.clear();` |
| `a[acnt++] = x;` | `v.push_back(x);` |

---

## 2. 만드는 다섯 가지 방법

```cpp
vector<int> a;                 // 길이 0. 나중에 채운다
vector<int> b(5);              // 5칸, 전부 0
vector<int> c(5, -1);          // 5칸, 전부 -1
int raw[4] = { 3, 1, 4, 1 };
vector<int> d(raw, raw + 4);   // 배열 구간을 그대로 복사  ★ 우리가 return에 쓰는 방식
vector<vector<int>> g(3, vector<int>(4, 7));   // 3행 4열, 전부 7
```

`d(raw, raw + 4)` 는 "**`raw[0]`부터 `raw[3]`까지**"라는 뜻이다.
끝은 **하나 더 뒤**를 가리킨다. `a[i] ~ a[j]`를 담고 싶으면 `vector<int>(a + i, a + j + 1)`.

---

## 3. 프로그래머스 제출에서 실제로 만나는 자리는 세 군데뿐

### (1) 시그니처 — 바꾸면 안 된다

```cpp
vector<int> solution(vector<vector<int>> city, vector<vector<int>> road)
```

이름과 타입이 채점기와 **글자 단위로** 맞아야 한다. 하나라도 다르면 이렇게 난다.

```
undefined reference to `solution(vector<vector<int>>, vector<vector<int>>)`
```

> 이 에러는 "solution을 못 찾겠다"는 뜻이다. `swtest/` 파일(= `main`만 있고 `solution`이 없는 파일)을
> 그대로 제출했을 때도 똑같이 난다.

### (2) `input()` — 인자를 내 전역 배열로 옮긴다

`scanf` 자리에 대입이 들어갈 뿐이다.

```cpp
// swtest 판
void input()
{
    scanf("%d %d", &N, &M);
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= M; c++)
            scanf("%d", &MAP[r][c]);
}

// 제출 판
void input(const vector<vector<int>>& board)
{
    N = (int)board.size();        // 행 수  = 바깥 vector 길이
    M = (int)board[0].size();     // 열 수  = 안쪽 vector 길이
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= M; c++)
            MAP[r][c] = board[r - 1][c - 1];   // 0-based -> 1-based
}
```

**여기서 끝이다.** 이 줄 이후로는 `MAP`만 보므로 로직 함수는 한 글자도 안 바뀐다.

### (3) `solution()` 마지막 줄 — 내 배열을 vector로 돌려준다

```cpp
// 답이 answer[1] ~ answer[N-1] 에 들어 있을 때
return vector<int>(answer + 1, answer + N);

// 답이 out[0] ~ out[ocnt-1] 에 들어 있을 때
return vector<int>(out, out + ocnt);

// 답이 정수 하나면 그냥
return count;
```

그래서 이 폴더 어디에도 `push_back`이 없다. 값은 늘 배열에 담고, 마지막에 한 줄로 옮긴다.

---

## 4. 인자는 복사된다 — `&`가 붙었는지 보라

```cpp
void f(vector<int> v);          // 통째로 복사한다 (원소 100만 개면 100만 개 복사)
void f(vector<int>& v);         // 원본을 가리킨다. 고치면 호출한 쪽도 바뀐다
void f(const vector<int>& v);   // 원본을 가리키되 못 고친다  ★ 읽기만 할 때 이걸 쓴다
```

우리 코드에서 `input()`이 `const vector<vector<int>>& board` 로 받는 이유가 이것이다.
`solution()`의 인자는 채점기가 정한 형태라 복사본이지만, 어차피 바로 `input()`으로 넘겨
전역 배열에 옮기므로 복사는 한 번뿐이다.

---

## 5. 꼭 알아야 할 함정 — `size()`는 부호 없는 타입

```cpp
vector<int> v;                  // 비어 있음. size() == 0

for (int i = 0; i < v.size() - 1; i++)   // ★ 무한 루프처럼 폭주한다
```

`size()`는 `size_t`(부호 없음)라서 `0 - 1`이 **-1이 아니라 4294967295**가 된다.
실제로 찍어 보면 이렇다.

```
empty size-1 = 4294967295  (int로 캐스팅하면 -1)
```

**예방법은 하나다. 받는 즉시 `int`로 바꿔 둔다.**

```cpp
int n = (int)v.size();
for (int i = 0; i < n - 1; i++) ...
```

우리 코드가 `N = (int)board.size();` 처럼 늘 `(int)`를 붙이는 이유다.

---

## 6. 2차원 vector

```cpp
vector<vector<int>> g(N, vector<int>(M, 0));   // N행 M열, 0으로 초기화

int rows = (int)g.size();        // 행 수
int cols = (int)g[0].size();     // 열 수  (행이 0개면 g[0]은 터진다)

g[r][c] = 5;                     // 접근은 배열과 동일
```

반환할 때 격자를 통째로 돌려줘야 하면 이렇게 만든다.

```cpp
vector<vector<int>> answer(N, vector<int>(M));
for (int r = 1; r <= N; r++)
    for (int c = 1; c <= M; c++)
        answer[r - 1][c - 1] = MAP[r][c];
return answer;
```

---

## 7. `string`은 `sscanf`와 같이 쓴다

명령 문자열을 받는 문제(스택·큐·덱)는 `vector<string>`으로 온다.
`substr`이나 `find` 같은 걸 새로 배울 필요 없이, **C로 읽으면 된다.**

```cpp
for (int i = 0; i < N; i++)
{
    char command[100];
    int value = 0;

    sscanf(commands[i].c_str(), "%s %d", command, &value);   // "push 10" 이면 둘 다 읽힌다

    if (strCompare(command, "push") == 0) stack[sp++] = value;
    ...
}
```

- `.c_str()` : `string` → `const char*` (C 함수에 넘기는 다리)
- 값이 없는 명령("pop")이면 `%d`가 실패하고 `value`는 그대로 0으로 남는다

---

## 8. 자주 보는 에러와 원인

| 에러 | 원인 |
|---|---|
| `undefined reference to solution(...)` | 제출 파일에 `solution`이 없거나 시그니처가 다름 |
| `multiple definition of main` | 제출 파일에 `main`이 남아 있음 (`local_test.cpp`를 같이 낸 경우) |
| `'vector' was not declared in this scope` | `#include <vector>` 누락 (또는 `using namespace std;` 없이 `std::` 생략) |
| `reference to 'move' is ambiguous` | 전역 이름이 `std::move` 등과 충돌 (→ 이름 바꾸기) |
| 런타임 에러 / 이상한 값 | `size() - 1` 부호 함정, 또는 `g[0]`을 빈 vector에서 읽음 |

---

## 9. 이 폴더에서 쓰는 관용구 모음 (그대로 외워 쓰면 된다)

```cpp
// 격자 받기
N = (int)board.size();
M = (int)board[0].size();
for (int r = 1; r <= N; r++)
    for (int c = 1; c <= M; c++)
        MAP[r][c] = board[r - 1][c - 1];

// 쿼리 목록 받기 (M줄 x 3개)
M = (int)queries.size();
for (int i = 0; i < M; i++) {
    int r = queries[i][0], c = queries[i][1], d = queries[i][2];
    ...
}

// 개수만 인자에서 얻고 나머지는 원본 그대로
M = (int)monsters.size();      // 원본의 scanf("%d", &M) 자리

// 정수 하나 반환
return simulate();

// 정수 여러 개 반환 (원본 출력 순서 그대로)
int out[3];
out[0] = roomCount; out[1] = maxRoom; out[2] = maxMerged;
return vector<int>(out, out + 3);

// 명령마다 한 줄씩 출력하던 문제
int out[MAX]; int ocnt = 0;
...
out[ocnt++] = stack[--sp];
...
return vector<int>(out, out + ocnt);
```

---

## 10. 스스로 점검 (답은 아래)

1. `vector<int> v(5, 3);` 의 `v.size()` 와 `v[4]` 는?
2. `int a[6] = {1,2,3,4,5,6};` 에서 `a[2]`부터 `a[4]`까지를 담은 vector를 만드는 한 줄은?
3. 아래 코드가 위험한 이유는?
   ```cpp
   for (int i = 0; i <= v.size() - 1; i++) printf("%d\n", v[i]);
   ```
4. `void f(vector<int> v)` 와 `void f(const vector<int>& v)` 의 차이는?
5. 채점기가 `undefined reference to solution(...)` 이라고 했다. 가장 먼저 확인할 것 두 가지는?
6. `vector<string> commands` 에서 `"push_front 7"` 의 명령어와 값을 C 스타일로 읽는 한 줄은?

<details>
<summary>답</summary>

1. `size()`는 **5**, `v[4]`는 **3**. (5칸을 전부 3으로 채운다)
2. `vector<int>(a + 2, a + 5)` — 끝은 하나 뒤를 가리킨다.
3. `v`가 비어 있으면 `v.size() - 1`이 **4294967295**가 되어 배열 밖을 마구 읽는다.
   `int n = (int)v.size();` 로 받아 두고 `i < n` 으로 쓴다.
4. 앞은 **복사본**을 받아 원본이 안 바뀌고 복사 비용이 든다. 뒤는 **원본을 가리키되 못 고친다**(읽기 전용, 복사 없음).
5. ① 제출한 파일에 `solution`이 있는지 (swtest 판을 낸 건 아닌지) ② 시그니처의 타입·인자 순서가 문제에서 준 것과 같은지.
6. `sscanf(commands[i].c_str(), "%s %d", command, &value);`

</details>

---

## 11. 정리

- `vector`는 **길이를 들고 다니는 배열**이다. `[]` 문법은 그대로다.
- 제출 코드에서 `vector`를 만나는 곳은 **시그니처 / `input()` / `return`** 세 군데뿐이다.
- 그 사이는 지금까지 하던 대로 전역 배열과 직접 만든 큐로 푼다.
- 딱 하나만 외운다면 : **`size()`는 받는 즉시 `(int)`로 캐스팅한다.**
