// NxN 격자 필요 define으로 숫자 넣고
// 0벽 1~4레일 (하좌상우 1234) 수하물 개수B 무게 w 스턴 t
// 1단계 이동 2단계 적재/ 파손여부 3단계 투입
// 풀이방침은 턴마다 3단계를 그대로 시뮬레이션
// bagMAP[r][c] = 그 칸에 있는 수하물 번호 0 은 빈칸
// 1단계 누가 움직이냐를 한꺼번에 정해서 한꺼번에 복사
// canMove(i) i가 이번턴에 움직일 수 있냐 
// 가려는 칸의 winner가 내가 아니면 -> 못움직임
// 가려는 칸이 비어있으면 움직임
// 가려는 칸에 수하물 j 가 있으면 canMove(j)를 따름
// 움직이는 수하물을 bagMAP에서 지운뒤 덮어 씌우기

#include <stdio.h>

#define MAX (8+2)
#define MAX_BAG (30+5)

#define WALL (0)
#define LOAD (9)

#define WAIT (0)
#define BELT (1)
#define DONE (2)    

int T;
int N, B;
int MAP[MAX][MAX];
int bagMAP[MAX][MAX];

struct BAG
{
    int t, r, c, w;       // 투입 예정 턴, 투입 칸, 무게
    int state;            // WAIT / BELT / DONE
    int stuck;            // 연속으로 못 움직인 턴 수
    int broken;           // 파손 여부
    int result;           // 적재 턴 (파손이면 음수)
};

BAG bag[MAX_BAG];

// -, 하, 좌, 상, 우
int dr[] = { 0,1,0,-1,0 };
int dc[] = { 0,0,-1,0,1 };

int winner[MAX][MAX];
int moveState[MAX_BAG];
int moved[MAX_BAG];

void input()
{
    scanf("%d", &N);

    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            MAP[r][c] = WALL, bagMAP[r][c] = 0;

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            scanf("%d", &MAP[r][c]);

    scanf("%d", &B);

    for (int i = 1; i <= B; i++)
    {
        scanf("%d %d %d %d", &bag[i].t, &bag[i].r, &bag[i].c, &bag[i].w);

        bag[i].state = WAIT;
        bag[i].stuck = 0;
        bag[i].broken = 0;
        bag[i].result = 0;
    }
}

int isPriority(int a, int b)
{
    if (bag[a].w != bag[b].w) return bag[a].w > bag[b].w;

    return a < b;
}

int canMove(int i)
{
    if (moveState[i] != -1) return moveState[i];

    int nr = bag[i].r + dr[MAP[bag[i].r][bag[i].c]];
    int nc = bag[i].c + dc[MAP[bag[i].r][bag[i].c]];

    if (winner[nr][nc] != i) return moveState[i] = 0;
    if (bagMAP[nr][nc] == 0) return moveState[i] = 1;
    return moveState[i] = canMove(bagMAP[nr][nc]);
}

// 1단계 이동
void moveBags()
{
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            winner[r][c] = 0;

    for (int i = 1; i <= B; i++)
    {
        moveState[i] = -1;
        moved[i] = 0;

        if (bag[i].state != BELT) continue;

        int nr = bag[i].r + dr[MAP[bag[i].r][bag[i].c]];
        int nc = bag[i].c + dc[MAP[bag[i].r][bag[i].c]];

        if (winner[nr][nc] == 0 || isPriority(i, winner[nr][nc])) winner[nr][nc] = i;
    }

    for (int i = 1; i <= B; i++)
        if (bag[i].state == BELT) moved[i] = canMove(i);

    for (int i = 1; i <= B; i++)
        if (moved[i]) bagMAP[bag[i].r][bag[i].c] = 0;
    
    for (int i = 1; i <= B; i++)
    {
        if (moved[i] == 0) continue;

        int d = MAP[bag[i].r][bag[i].c];
        bag[i].r += dr[d];
        bag[i].c += dc[d];

        bagMAP[bag[i].r][bag[i].c] = i;
    }
}

// 2단계
void loadAndCheck(int turn)
{
    for (int i = 1; i <= B; i++)
    {
        if (bag[i].state != BELT) continue;

        if (MAP[bag[i].r][bag[i].c] == LOAD)
        {
            bag[i].state = DONE;
            bag[i].result = bag[i].broken ? -turn : turn;
            bagMAP[bag[i].r][bag[i].c] = 0;
            continue;
        }

        if (moved[i]) bag[i].stuck = 0;
        else
        {
            bag[i].stuck++;
            if (bag[i].stuck >= 3) bag[i].broken = 1;
        }
    }
}

// 3단계
void insertBags(int turn)
{
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
        {
            if (bagMAP[r][c] != 0) continue;

            int best = 0;
            for (int i = 1; i <= B; i++)
            {
                if (bag[i].state != WAIT || bag[i].t > turn) continue;
                if (bag[i].r != r || bag[i].c != c) continue;

                if (best == 0 || bag[i].t < bag[best].t) best = i;
            }

            if (best == 0) continue;

            bag[best].state = BELT;
            bag[best].stuck = 0;
            bagMAP[r][c] = best;
        }
    }
}

int isFinished()
{
    for (int i = 1; i <= B; i++)
        if (bag[i].state != DONE) return 0;
    return 1;
}

void simulate()
{
    for (int turn = 1; isFinished() == 0; turn++)
    {
        moveBags();
        loadAndCheck(turn);
        insertBags(turn);
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

        for (int i = 1; i <= B; i++)
            printf("%d ", bag[i].result);
        putchar('\n');
    }
    return 0;
}