#include <stdio.h>

#define MAX (8 + 5)
#define MAX_B (30 + 5)

// 풀이: 목적지가 겹치면 무게와 번호로 후보를 먼저 하나만 남긴다.
// 그 후보가 실제 이동할 수 있는지는 앞 칸의 수하물부터 재귀적으로 판정한다.
// 모든 레일은 적재 장소로 이어지므로 이동 의존 관계에 순환이 없다.
int T;
int N, B, loaded;
int MAP[MAX][MAX];
int occupied[MAX][MAX];
int winner[MAX][MAX];
int candidate[MAX][MAX];

// 0, 아래, 왼쪽, 위, 오른쪽
int dr[] = { 0, 1, 0, -1, 0 };
int dc[] = { 0, 0, -1, 0, 1 };

struct BAG
{
    int t; // 투입 예정 턴
    int r;
    int c;
    int w; // 무게
    int stopped; // 연속으로 이동하지 못한 턴
    int answer; // 적재 턴 (파손되면 음수)
    bool entered;
    bool delivered;
    bool broken;
};

BAG bag[MAX_B];
int nextR[MAX_B], nextC[MAX_B];
int moveState[MAX_B]; // 0: 미확인, 1: 이동 불가, 2: 이동 가능

void input()
{
    scanf("%d", &N);

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            scanf("%d", &MAP[r][c]);

    scanf("%d", &B);
    for (int i = 1; i <= B; i++)
    {
        scanf("%d %d %d %d", &bag[i].t, &bag[i].r, &bag[i].c, &bag[i].w);
        bag[i].stopped = bag[i].answer = 0;
        bag[i].entered = bag[i].delivered = bag[i].broken = false;
    }

    loaded = 0;
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            occupied[r][c] = 0;
}

bool isPriority(int a, int b)
{
    if (b == 0) return true;
    if (bag[a].w != bag[b].w) return bag[a].w > bag[b].w;
    return a < b;
}

bool canMove(int index)
{
    if (moveState[index] != 0) return moveState[index] == 2;

    int r = nextR[index];
    int c = nextC[index];

    // 같은 칸을 노린 수하물 중 우선순위에서 밀리면 이동할 수 없다.
    if (winner[r][c] != index)
    {
        moveState[index] = 1;
        return false;
    }

    // 앞 수하물이 비켜야 현재 수하물도 그 칸에 들어갈 수 있다.
    int front = occupied[r][c];
    if (front != 0 && canMove(front) == false)
    {
        moveState[index] = 1;
        return false;
    }

    moveState[index] = 2;
    return true;
}

void moveBags(int turn)
{
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            winner[r][c] = 0;

    // 현재 위치를 바꾸기 전에 각 목적지의 이동 후보를 확정한다.
    for (int i = 1; i <= B; i++)
    {
        moveState[i] = 0;
        if (bag[i].entered == false || bag[i].delivered == true) continue;

        int dir = MAP[bag[i].r][bag[i].c];
        nextR[i] = bag[i].r + dr[dir];
        nextC[i] = bag[i].c + dc[dir];

        int &other = winner[nextR[i]][nextC[i]];
        if (isPriority(i, other) == true) other = i;
    }

    // 원래 점유 상태를 기준으로 모든 수하물의 이동 여부를 결정한다.
    for (int i = 1; i <= B; i++)
        if (bag[i].entered == true && bag[i].delivered == false)
            canMove(i);

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            occupied[r][c] = 0;

    // 이동 결과를 한꺼번에 반영하고, 적재 또는 연속 정지에 따른 파손을 처리한다.
    for (int i = 1; i <= B; i++)
    {
        if (bag[i].entered == false || bag[i].delivered == true) continue;

        if (moveState[i] == 2)
        {
            bag[i].r = nextR[i];
            bag[i].c = nextC[i];
            bag[i].stopped = 0;
        }
        else
        {
            bag[i].stopped++;
            if (bag[i].stopped >= 3) bag[i].broken = true;
        }

        if (MAP[bag[i].r][bag[i].c] == 9)
        {
            bag[i].delivered = true;
            bag[i].answer = bag[i].broken ? -turn : turn;
            loaded++;
        }
        else occupied[bag[i].r][bag[i].c] = i;
    }
}

void enterBags(int turn)
{
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            candidate[r][c] = 0;

    // 아직 투입되지 않은 수하물 중 칸마다 예정 턴이 가장 이른 하나를 고른다.
    for (int i = 1; i <= B; i++)
    {
        if (bag[i].entered == true || bag[i].t > turn) continue;

        int r = bag[i].r;
        int c = bag[i].c;
        if (occupied[r][c] != 0) continue;

        int &other = candidate[r][c];
        if (other == 0 || bag[i].t < bag[other].t) other = i;
    }

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
        {
            int index = candidate[r][c];
            if (index == 0) continue;
            bag[index].entered = true;
            occupied[r][c] = index;
        }
}

void simulate()
{
    // 투입은 이동과 적재가 끝난 뒤에 하므로 새 수하물은 다음 턴부터 이동한다.
    for (int turn = 1; loaded < B; turn++)
    {
        // 1단계: 현재 벨트 위 수하물을 동시에 이동
        moveBags(turn);

        // 2단계: 적재와 파손은 moveBags에서 처리

        // 3단계: 아직 투입되지 않은 수하물 투입
        enterBags(turn);
    }

    for (int i = 1; i <= B; i++)
        printf("%d%c", bag[i].answer, i == B ? '\n' : ' ');
}

int main()
{
    // scanf("%d", &T);
    T = 1;
    for (int tc = 1; tc <= T; tc++)
    {
        input();
        simulate();
    }

    return 0;
}