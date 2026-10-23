/*
    [코드트리] 2026 하반기 - 코디의 퍼즐 공방

    ■ 문제 요약
      N x N 퍼즐 판(3 <= N <= 2,000). 퍼즐의 네 변(위, 오른쪽, 아래, 왼쪽)은 0 오목, 1 직선, 2 볼록.
      맞물림 : 테두리에 닿은 변은 직선, 퍼즐이 놓인 이웃과 맞닿은 변은 맞물려야 한다 (0 <-> 2, 1 <-> 1).
      구멍 : 네 방향이 모두 "테두리 또는 퍼즐이 놓인 칸"인 빈칸. 들어가야 할 모양이 하나로 정해진다.
      스페어 퍼즐을 0 ~ 3번 돌려 그 모양이 되면 그 구멍에 넣을 수 있다 (여러 방향으로 맞아도 1개로 센다).
      점수 = 모든 구멍에 대해 (넣을 수 있는 스페어 개수)의 합.
      명령 Q개(최대 100,000) : 100 판 준비 / 200 배치 / 300 제거 / 400 스페어 추가 / 500 점수 출력.

      입출력 예
          예제 1 -> 2 3 0 1
          예제 2 -> 1 2 5

    ■ 판을 매번 훑으면 안 된다
      500 명령마다 400만 칸을 다 보면 10만 번 x 400만 = 4천억 번이라 시간 초과다.
      그래서 "구멍마다 들어가야 할 모양"을 모양별 개수 holeCnt[]로 늘 최신으로 들고 있고,
      500 명령에서는 모양 81가지만 본다.

    ■ 모양을 번호 하나로 : 위 x 27 + 오른쪽 x 9 + 아래 x 3 + 왼쪽  (0 ~ 80)
      각 변이 0 ~ 2라 3진수 네 자리로 보면 된다.
      시계 방향 회전 : a b c d -> d a b c

    ■ 1) 구멍 개수를 칸이 바뀔 때만 고친다
      어느 칸이 구멍인지, 무슨 모양이 필요한지는 "자기 칸 + 상하좌우 이웃"만 보고 정해진다.
      그래서 (y, x)에 놓거나 치우면 영향을 받는 칸은 (y, x)와 그 네 이웃, 다섯 칸뿐이다.
          다섯 칸마다 바뀌기 전 구멍 모양을 holeCnt에서 빼고
          -> 판을 바꾸고
          -> 다섯 칸마다 바뀐 뒤 구멍 모양을 holeCnt에 더한다
      명령 하나에 상수 시간이다.

    ■ 2) 점수 = 모양별로 (구멍 수) x (그 모양에 맞는 스페어 수)
      스페어 P가 구멍 모양 S에 맞는다  <=>  P를 돌려 S가 된다  <=>  S를 돌려 P가 된다.
      그래서 fit(S) = S를 0 ~ 3번 돌린 모양들 중 "서로 다른" 것마다 spareCnt를 더한 값.
      (0 1 0 1처럼 돌려서 같은 모양이 또 나오면 한 번만 더한다 = 스페어 하나를 한 번만 센다)
          점수 = 모든 모양 S에 대해 holeCnt[S] x fit(S)
      500 명령마다 81 x 4번 계산이면 끝난다.

    ■ 구멍에 들어가야 할 모양 (getHole)
      방향마다 이웃이 판 밖이면 직선(1), 퍼즐이 있으면 "그 퍼즐의 맞닿은 변"의 반대(2 - 변).
      빈 이웃이 하나라도 있으면 구멍이 아니다 (-1).

    ■ 주의할 점
      - 점수는 long long. 구멍은 최대 200만 개 정도, 스페어는 최대 10만 개라 int를 넘을 수 있다.
      - 판은 int [2002][2002] (약 16MB)라 전역에 둔다.
      - 배치 명령의 모양은 늘 맞물림 규칙을 만족한다고 했으므로 검사하지 않는다.
      - 300으로 치운 퍼즐은 스페어가 되지 않는다. 스페어는 400으로만 늘고 줄지 않는다.
*/
#include <stdio.h>

#define MAX (2000+5)
#define SHAPE (81)
#define EMPTY (-1)

int T;
int N, Q;

int board[MAX][MAX];        // 칸에 놓인 퍼즐 모양 번호 (EMPTY = 빈칸)
long long holeCnt[SHAPE];   // holeCnt[S] : 모양 S가 들어가야 하는 구멍 수
long long spareCnt[SHAPE];  // spareCnt[S] : 모양 S인 스페어 수

// 위, 오른쪽, 아래, 왼쪽
int dr[] = { -1,0,1,0 };
int dc[] = { 0,1,0,-1 };

int makeShape(int u, int r, int d, int l)
{
    return u * 27 + r * 9 + d * 3 + l;
}

// 모양 번호에서 dir 방향 변 (0 위, 1 오른쪽, 2 아래, 3 왼쪽)
int getSide(int shape, int dir)
{
    int div[] = { 27, 9, 3, 1 };
    return shape / div[dir] % 3;
}

// 시계 방향 90도 : a b c d -> d a b c
int rotate(int shape)
{
    return makeShape(getSide(shape, 3), getSide(shape, 0), getSide(shape, 1), getSide(shape, 2));
}

// (r, c)가 구멍이면 들어가야 할 모양 번호, 아니면 -1
int getHole(int r, int c)
{
    if (r < 1 || c < 1 || r > N || c > N) return -1;   // 판 밖
    if (board[r][c] != EMPTY) return -1;               // 퍼즐이 있다

    int side[4];
    for (int dir = 0; dir < 4; dir++)
    {
        int nr = r + dr[dir];
        int nc = c + dc[dir];

        if (nr < 1 || nc < 1 || nr > N || nc > N) side[dir] = 1;   // 테두리 -> 직선
        else if (board[nr][nc] == EMPTY) return -1;                // 빈 이웃 -> 구멍 아님
        else side[dir] = 2 - getSide(board[nr][nc], (dir + 2) % 4);   // 맞닿은 변의 반대
    }

    return makeShape(side[0], side[1], side[2], side[3]);
}

// (y, x)와 네 이웃의 구멍 기여를 holeCnt에 더하거나(+1) 뺀다(-1)
void updateAround(int y, int x, int sign)
{
    int hole = getHole(y, x);
    if (hole != -1) holeCnt[hole] += sign;

    for (int dir = 0; dir < 4; dir++)
    {
        hole = getHole(y + dr[dir], x + dc[dir]);
        if (hole != -1) holeCnt[hole] += sign;
    }
}

// 100 : 판 준비
void prepare(int n)
{
    N = n;

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            board[r][c] = EMPTY;

    for (int s = 0; s < SHAPE; s++)
        holeCnt[s] = spareCnt[s] = 0;

    // 처음엔 모든 칸이 비어 있어 구멍이 없다 (N >= 3이라 모든 칸에 빈 이웃이 있다)
}

// 200 : 배치 / 300 : 제거  -> 바뀌기 전 기여를 빼고, 바꾸고, 바뀐 뒤 기여를 더한다
void setCell(int y, int x, int shape)
{
    updateAround(y, x, -1);
    board[y][x] = shape;
    updateAround(y, x, +1);
}

// 500 : 점수
long long getScore()
{
    long long score = 0;

    for (int s = 0; s < SHAPE; s++)
    {
        if (holeCnt[s] == 0) continue;

        // s를 0 ~ 3번 돌린 모양 중 서로 다른 것마다 스페어 수를 더한다
        int rot[4];
        long long fit = 0;

        rot[0] = s;
        for (int i = 1; i < 4; i++) rot[i] = rotate(rot[i - 1]);

        for (int i = 0; i < 4; i++)
        {
            int dup = 0;
            for (int j = 0; j < i; j++)
                if (rot[j] == rot[i]) dup = 1;   // 이미 센 모양

            if (dup == 0) fit += spareCnt[rot[i]];
        }

        score += holeCnt[s] * fit;
    }

    return score;
}

int main()
{
    //scanf("%d", &T);
    T = 1;
    for (int tc = 1; tc <= T; tc++)
    {
        scanf("%d", &Q);

        for (int q = 0; q < Q; q++)
        {
            int cmd;
            scanf("%d", &cmd);

            if (cmd == 100)
            {
                int n;
                scanf("%d", &n);
                prepare(n);
            }
            else if (cmd == 200)
            {
                int y, x, u, r, d, l;
                scanf("%d %d %d %d %d %d", &y, &x, &u, &r, &d, &l);
                setCell(y, x, makeShape(u, r, d, l));
            }
            else if (cmd == 300)
            {
                int y, x;
                scanf("%d %d", &y, &x);
                setCell(y, x, EMPTY);
            }
            else if (cmd == 400)
            {
                int u, r, d, l;
                scanf("%d %d %d %d", &u, &r, &d, &l);
                spareCnt[makeShape(u, r, d, l)]++;
            }
            else if (cmd == 500)
            {
                printf("%lld\n", getScore());
            }
        }
    }

    return 0;
}
