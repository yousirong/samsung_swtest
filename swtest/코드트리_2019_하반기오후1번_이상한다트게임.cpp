/*
    [코드트리] 2019 하반기 오후 1번 - 이상한 다트 게임
    (백준 17822 "원판 돌리기"와 같은 문제다  https://www.acmicpc.net/problem/17822)

    ■ 문제 요약
      원판이 N개 있고(1번이 가장 안쪽), 각 원판에는 수가 M개씩 원형으로 적혀 있다.
      회전 명령 Q개가 (X, D, K) 형태로 주어진다.

        X : X의 배수 번호인 원판들을 회전
        D : 0이면 시계, 1이면 반시계
        K : K칸 회전

      한 번 회전할 때마다 아래 후처리를 한다.

        1) 인접하면서 수가 같은 칸들을 모두 찾아 지운다(0으로 만든다).
           인접이란 같은 원판의 양옆(1번과 M번도 이어져 있다)과, 위/아래 원판의 같은 위치다.
        2) 지워진 것이 하나도 없으면, 남은 수들의 평균을 구해
           평균보다 큰 수는 1 줄이고, 작은 수는 1 늘린다.

      Q번을 모두 수행한 뒤 남아 있는 수의 합을 출력한다.

    ■ 구조
      원판을 2차원 배열 circle[원판][위치]로 두면, 위아래 인접은 그냥 상하 이동이고
      양옆 인접은 좌우 이동인데 1번과 M번만 따로 이어 주면 된다.

        rotate()      : 조건에 맞는 원판들을 K칸 돌린다
        BFS()         : 인접하고 값이 같은 덩어리를 찾아 크기가 2 이상이면 지운다
        makeAverage() : 지운 것이 없을 때 평균 쪽으로 한 칸씩 당긴다
        startGame()   : 위 두 후처리를 순서대로 조율한다

    ■ 회전
      13번 파일(배열 돌리기)과 같은 방식이다.
      원판 하나를 arr에 통째로 복사해 두고, 새 위치 i가 옛 위치 i-k의 값을 가져온다.

        - 반시계는 시계의 반대이므로 k에 부호만 뒤집어 쓴다
        - C의 %는 음수를 그대로 음수로 돌려주므로 반드시 + index 보정을 해야 한다

    ■ 인접 판정의 경계 처리
      circle 배열을 통째로 0으로 밀어 두었기 때문에, 위/아래로 원판 범위를 벗어나면
      값이 0이 되어 "이미 지워진 칸"과 같은 취급으로 자동으로 걸러진다.
      좌우도 마찬가지로 0번/M+1번 칸이 0이라 그냥 넘어가고,
      진짜 이웃인 1번 <-> M번 연결만 BFS 안에서 따로 처리한다.

    ■ [주의] 평균 조정이 정수 평균으로 되어 있다
      문제에서 말하는 평균은 실수 평균이다. 그런데 이 코드는

          avg = sum / count;      // 정수 나눗셈이라 소수점이 잘린다

      로 구한 뒤 그 값과 비교한다. 그래서 평균이 딱 떨어지지 않는 입력에서 답이 달라진다.

        예) 남은 수가 1과 2뿐이라면 실제 평균은 1.5이므로 1 -> 2, 2 -> 1 이 되어야 한다.
            정수 평균은 1이므로 이 코드는 1은 그대로 두고 2만 1로 줄여 버린다.

      소수점 오차 없이 고치려면 나눗셈 대신 곱셈으로 비교하면 된다.

          if (circle[r][c] * count < sum) circle[r][c]++;
          else if (circle[r][c] * count > sum) circle[r][c]--;
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (50 + 5)

// 회전 방향
#define CLOCKWISE (0)
#define COUNTER_CLOCKWISE (1)

int T;
int N, M, Q;

// circle[r][c] : r번 원판의 c번 위치에 적힌 수 (0이면 지워진 자리)
int circle[MAX][MAX];

// BFS 방문 표시
int visit[MAX][MAX];

// 회전 명령
int X[MAX];
int D[MAX];
int K[MAX];

// 좌표 한 쌍
struct RC
{
    int r;
    int c;
};

typedef struct RC RC;

RC queue[MAX * MAX];   // BFS용 큐

// 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력
// ---------------------------
void input()
{
    scanf("%d %d %d", &N, &M, &Q);

    // 배열 전체를 0으로 밀어 둔다.
    // 원판 범위 밖이 0이 되어 인접 판정에서 자동으로 걸러진다.
    for (int r = 0; r < MAX; r++)
        for (int c = 0; c < MAX; c++)
            circle[r][c] = 0;

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= M; c++)
            scanf("%d", &circle[r][c]);

    for (int i = 1; i <= Q; i++)
        scanf("%d %d %d", &X[i], &D[i], &K[i]);
}

// ---------------------------
// 디버그용 출력
// ---------------------------
void printMap()
{
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= M; c++)
            printf("%d ", circle[r][c]);
        putchar('\n');
    }
    putchar('\n');
}

// ---------------------------
// x의 배수 번호 원판들을 d 방향으로 k칸 회전
//
// 원판 하나를 arr에 복사해 두고 "새 위치가 옛 위치의 값을 가져오는" 방식으로 채운다.
// ---------------------------
void rotate(int x, int d, int k)
{
    int arr[MAX] = { 0 };

    // 반시계는 시계의 반대이므로 부호만 뒤집으면 같은 식을 쓸 수 있다
    if (d == COUNTER_CLOCKWISE) k = -k;

    for (int r = x; r <= N; r += x)
    {
        int index = 0;

        // 회전 전 상태를 통째로 떠 놓는다 (덮어쓰기 사고 방지)
        for (int c = 1; c <= M; c++)
            arr[index++] = circle[r][c];

        for (int i = 0; i < index; i++)
        {
            int newIndex = (i - k) % index;

            // C의 %는 음수를 음수로 돌려주므로 보정이 필요하다
            newIndex = (newIndex < 0) ? newIndex + index : newIndex;

            circle[r][i + 1] = arr[newIndex];
        }
    }
}

// ---------------------------
// (r, c)와 이어진 "같은 수" 덩어리를 찾아 크기가 2 이상이면 모두 지운다
//
//   반환값 : 실제로 지웠으면 true
// ---------------------------
bool BFS(int r, int c)
{
    int rp, wp;
    rp = wp = 0;

    queue[wp].r = r;
    queue[wp++].c = c;

    visit[r][c] = true;

    while (rp < wp)
    {
        RC out = queue[rp++];

        // 상하좌우. 원판 범위 밖은 값이 0이라 아래 조건에서 걸러진다.
        for (int i = 0; i < 4; i++)
        {
            int nr = out.r + dr[i];
            int nc = out.c + dc[i];

            // 이미 지워진 칸(또는 범위 밖)
            if (circle[nr][nc] == 0) continue;

            // 값이 다르거나 이미 이 덩어리에 넣은 칸
            // (덩어리 안의 값은 모두 같으므로 시작 칸의 값과 비교해도 된다)
            if (circle[nr][nc] != circle[r][c] || visit[nr][nc] == true) continue;

            queue[wp].r = nr;
            queue[wp++].c = nc;

            visit[nr][nc] = true;
        }

        // 원판은 원형이므로 1번과 M번도 이웃이다. 위 4방향으로는 잡히지 않아 따로 처리한다.
        if (out.c == 1)
        {
            if (circle[r][c] == circle[out.r][M] && visit[out.r][M] == false)
            {
                queue[wp].r = out.r;
                queue[wp++].c = M;

                visit[out.r][M] = true;
            }
        }
        else if (out.c == M)
        {
            if (circle[r][c] == circle[out.r][1] && visit[out.r][1] == false)
            {
                queue[wp].r = out.r;
                queue[wp++].c = 1;

                visit[out.r][1] = true;
            }
        }
    }

    // 혼자면 지우지 않는다. 두 칸 이상 이어졌을 때만 삭제 대상이다.
    if (wp > 1)
    {
        for (int i = 0; i < wp; i++)
        {
            int rr = queue[i].r;
            int cc = queue[i].c;

            circle[rr][cc] = 0;
        }
        return true;
    }

    return false;
}

// ---------------------------
// 지운 것이 없을 때의 평균 조정
//
// 남은 수들의 평균을 구해 큰 수는 1 줄이고 작은 수는 1 늘린다.
//
// [주의] 파일 상단에 적었듯이 avg가 정수 나눗셈이라 실수 평균과 다르다.
//        정확히 하려면 나눗셈 대신 circle * count 와 sum 을 비교해야 한다.
// ---------------------------
void makeAverage()
{
    int sum = 0;
    int count = 0;
    int avg;

    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= M; c++)
        {
            if (circle[r][c] != 0)
            {
                sum += circle[r][c];
                count++;
            }
        }
    }

    // 남은 수가 하나도 없으면 조정할 것도 없다 (0으로 나누는 것도 막아 준다)
    if (count == 0) return;

    avg = sum / count;

    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= M; c++)
        {
            if (circle[r][c] == 0) continue;

            if (circle[r][c] < avg) circle[r][c]++;
            else if (circle[r][c] > avg) circle[r][c]--;
            // 평균과 같으면 그대로 둔다
        }
    }
}

// ---------------------------
// 회전 한 번 뒤의 후처리
//
//   모든 칸에서 덩어리를 찾아 지우고, 한 번도 못 지웠으면 평균 조정을 한다.
// ---------------------------
void startGame()
{
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= M; c++)
            visit[r][c] = false;

    bool deleteCheck = false;

    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= M; c++)
        {
            // 이미 지워졌거나 다른 덩어리에 포함된 칸은 시작점이 될 수 없다
            if (circle[r][c] == 0 || visit[r][c] == true) continue;

            bool check = BFS(r, c);
            if (check == true) deleteCheck = true;
        }
    }

    // 한 덩어리도 못 지웠을 때만 평균 조정을 한다
    if (deleteCheck == false) makeAverage();
}

// ---------------------------
// Q개의 회전 명령을 순서대로 수행
// ---------------------------
void simulate()
{
    for (int q = 1; q <= Q; q++)
    {
        rotate(X[q], D[q], K[q]);
        startGame();
    }
}

// ---------------------------
// 남아 있는 수의 합 (지워진 칸은 0이라 그냥 다 더하면 된다)
// ---------------------------
int getAnswer()
{
    int sum = 0;

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= M; c++)
            sum += circle[r][c];

    return sum;
}

int main()
{
    // 이 문제는 테스트 케이스가 하나다
    //scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();

        simulate();

        printf("%d\n", getAnswer());
    }

    return 0;
}
