#include <stdio.h>    

#define MAX (50+5)

#define ROAD (0)
#define STREET (1)
#define MARK (2)

int T;
int N, M, R, C, D;
int MAP[MAX][MAX];

// 북 동 남 서
int dr[] = { -1,0,1,0 };
int dc[] = { 0,1,0,-1 };

void input()
{
    scanf("%d %d %d %d %d", &N, &M, &R, &C, &D);

    for (int r = 0; r < N; r++)
        for (int c = 0; c < M; c++)
            scanf("%d", &MAP[r][c]);
}

void printMap() // for debug
{
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < M; c++)
            printf("%d ", MAP[r][c]);
        putchar('\n');
    }
    putchar('\n');
}

void simulate()
{
    int changeDir[] = { 3,0,1,2 };

    while (1)
    {
        MAP[R][C] = MARK;

        int nDir = changeDir[D];
        int nr = R + dr[nDir];
        int nc = C + dc[nDir];

        if (MAP[nr][nc] == ROAD)
        {
            D = nDir;
            R = nr;
            C = nc;
        }
        else
        {
            int i;
            for (i = 0; i < 4; i++)
            {
                D = nDir;
                int nr = R + dr[D];
                int nc = C + dc[D];

                if (MAP[nr][nc] == ROAD)
                {
                    R = nr;
                    C = nc;
                    break;
                }
                nDir = changeDir[D];
            }

            if (i == 4)
            {
                nr = R - dr[D];
                nc = C - dc[D];

                R = nr;
                C = nc;

                if (MAP[R][C] == STREET) break;
            }
        }
    }
}

int getAnswer()
{
    int sum = 0;
    for (int r = 0; r < N; r++)
        for (int c = 0; c < M; c++)
            if (MAP[r][c] == MARK) sum++;
    return sum;
}

int main()
{
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