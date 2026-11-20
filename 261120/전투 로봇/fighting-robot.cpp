#include <stdio.h>

#define MAX (20+5)
#define INF (0x7fff0000)

#define ROBOT_POSITION (9)

int T;
int N;
int MAP[MAX][MAX];
int visit[MAX][MAX];

struct ROBOT
{
    int r;
    int c;
    int attack;
    int level;
};

ROBOT battleRobot;

struct RC
{
    int r;
    int c;
};

RC queue[MAX * MAX];

// 상 우 하 좌
int dr[] = { -1,0,1,0 };
int dc[] = { 0,1,0,-1 };

void input()
{
    scanf("%d", &N);

    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
        {
            scanf("%d", &MAP[r][c]);

            if (MAP[r][c] == ROBOT_POSITION)
            {
                MAP[r][c] = 0;

                battleRobot.r = r;
                battleRobot.c = c;
                battleRobot.attack = 0;
                battleRobot.level = 2;
            }
        }
    }
}

void printMap()
{
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
        {
            printf("%d ", visit[r][c]);
        }
        putchar('\n');
    }
    putchar('\n');
}


bool monsterExists()
{
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            if (MAP[r][c] != 0 && MAP[r][c] < battleRobot.level) return true;
    return false;
}

void BFS()
{
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            visit[r][c] = 0;

    int rp, wp;

    rp = wp = 0;
    
    queue[wp].r = battleRobot.r;
    queue[wp++].c = battleRobot.c;

    visit[battleRobot.r][battleRobot.c] = 1;

    while (rp < wp)
    {
        RC out = queue[rp++];

        for (int i = 0; i < 4; i++)
        {
            int nr = out.r + dr[i];
            int nc = out.c + dc[i];

            if (nr<1 || nc<1 || nr>N || nc>N) continue;

            if (battleRobot.level < MAP[nr][nc]) continue;
            if (visit[nr][nc] != 0) continue;

            queue[wp].r = nr;
            queue[wp++].c = nc;

            visit[nr][nc] = visit[out.r][out.c] + 1;
        }
    }
}

int simulate()
{
    if (monsterExists() == false) return 0;

    BFS();

    int tmpR, tmpC;
    int minTime = INF;

    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
        {
            if (MAP[r][c] == 0 || visit[r][c] == 0) continue;
            if (battleRobot.level <= MAP[r][c]) continue;

            if (visit[r][c] < minTime)
            {
                minTime = visit[r][c];
                tmpR = r;
                tmpC = c;
            }
        }
    }

    // printMap();
    if (minTime == INF) return 0;
    // 몬스터를 없애면 해당 칸은 빈칸
    MAP[tmpR][tmpC] = 0;
    // 로봇이동 및 제거한 몬스터수 증가
    battleRobot.r = tmpR;
    battleRobot.c = tmpC;
    battleRobot.attack++;

    if (battleRobot.attack == battleRobot.level)
    {
        battleRobot.level++;
        battleRobot.attack = 0;
    }
    return minTime - 1;

}

int main()
{
    //scanf("%d", &T);
    T = 1;
    for (int tc = 1; tc <= T; tc++)
    {
        input();

        int time = 0;

        while (1)
        {
            int result = simulate();

            if (result == 0)break;

            time += result;
        }
        printf("%d\n", time);
    }
    return 0;
}