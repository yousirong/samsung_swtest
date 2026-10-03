#include <stdio.h>    

#define MAX (50+5)
#define INF (0x7fff0000)
#define ROCK (1)

#define UP (1)
#define DOWN (2)
#define LEFT (3)
#define RIGHT (4)

int T;

int N, R, C, D;

int MAP[MAX][MAX];
int check[MAX][MAX];

// 0, 상, 하, 좌, 우
int dr[] = { 0,-1,1,0,0 };
int dc[] = { 0,0,0,-1,1 };

// D = 1 -> 1, 3, 4, 2
// D = 2 -> 2, 4, 3, 1
// D = 3 -> 3, 2, 1, 4
// D = 4 -> 4, 1, 2, 3

int changeDirection[5][5] = {
    {0, 0, 0, 0, 0},
    {0, 1, 3, 4, 2},
    {0, 2, 4, 3, 1},
    {0, 3, 2, 1, 4},
    {0, 4, 1, 2, 3},
};

struct RCD
{
    int r;
    int c;
    int depth;
    int dir;
};

RCD queue[MAX * MAX];
int rp, wp;

void input()
{
    scanf("%d %d %d %d", &N, &R, &C, &D);

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            scanf("%d", &MAP[r][c]);
}

void printMap(int map[MAX][MAX])
{
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; c++)
            printf("%d ", map[r][c]);
        putchar('\n');
    }
    putchar('\n');
}

bool explore()
{
    for (int i = 1; i <= 4; i++)
    {
        int dir = changeDirection[D][i];
        int nr, nc;

        nr = R + dr[dir];
        nc = C + dc[dir];

        if (nr<1 || nc<1 || nr>N || nc>N) continue;
        if (MAP[nr][nc] == ROCK || check[nr][nc] == 1) continue;

        check[nr][nc] = 1;

        R = nr;
        C = nc;
        D = dir;

        return true;
    }
    return false;
}

bool isPriority(RCD a, RCD b)
{
    if (a.depth != b.depth) return a.depth < b.depth;
    if (a.r != b.r) return a.r < b.r;
    return a.c < b.c;
}

RCD getNextStep(int sr, int sc) //BFS
{
    RCD ret;
    int visit[MAX][MAX] = { 0 };

    rp = wp = 0;

    queue[wp].r = sr;
    queue[wp].c = sc;
    queue[wp++].depth = 0;

    visit[sr][sc] = 1;

    int priorityDir[] = { LEFT, DOWN, RIGHT, UP };
    while (rp < wp)
    {
        RCD out = queue[rp++];

        for (int i = 0; i < 4; i++)
        {
            int nr, nc;

            nr = out.r + dr[priorityDir[i]];
            nc = out.c + dc[priorityDir[i]];

            if (nr<1 || nc<1 || nr>N || nc>N)continue;
            if (visit[nr][nc] == 1 || MAP[nr][nc] == ROCK) continue;

            queue[wp].r = nr;
            queue[wp].c = nc;
            queue[wp].depth = out.depth + 1;

            int dir = 0;
            if (nr - out.r == -1) dir = UP;
            else if (nr - out.r == 1) dir = DOWN;
            else if (nc - out.c == 1) dir = RIGHT;
            else dir = LEFT;

            queue[wp++].dir = dir;
            visit[nr][nc] = 1;
        }

        ret.r = ret.c = ret.depth = INF;
        for (int i = 0; i < wp; i++)
        {
            RCD tmp = queue[i];

            int r, c;
            r = tmp.r;
            c = tmp.c;

            if (check[r][c] == 1)continue;

            if (isPriority(tmp, ret) == true)
                ret = tmp;
        }
        
    }
    return ret;
}

void simulate()
{
    check[R][C] = 1;

    printf("%d %d\n", R, C);

    for (int k = 0; k < N * N; k++)
    {
        // 1단계 : 인접 탐험
        bool isExplore = explore();
        if (isExplore == true)
        {
            printf("%d %d\n", R, C);
            continue;
        }

        // 2단계 : 가장 가까운 바다로 이동
        RCD next = getNextStep(R, C);

        if (next.r == INF) return;

        R = next.r;
        C = next.c;
        D = next.dir;

        check[R][C] = 1;

        printf("%d %d\n", R, C);
        //printMap(check);
    }
}

int main()
{
    //scanf("%d", &T);
    T = 1;
    for (int tc = 1; tc <= T; tc++)
    {
        input();

        simulate();
    }
    return 0;
}