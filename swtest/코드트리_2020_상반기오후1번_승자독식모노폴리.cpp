/*
    [코드트리] 2020 상반기 오후 1번 - 승자독식 모노폴리

    ■ 문제 요약
      N x N 격자 위에 플레이어 M명이 있다. 각자 현재 방향과 "방향 우선순위 표"를 가진다.
      플레이어가 어떤 칸에 있으면 그 칸에 자기 독점 계약을 남기고, 계약은 K초 동안 유지된다.

      1초마다 아래가 순서대로 일어난다.

        1) 살아 있는 플레이어가 각자 지금 서 있는 칸에 독점 계약을 새로 남긴다(남은 시간 K).
        2) 각 플레이어가 한 칸 이동한다. 이동할 칸은 자기 우선순위 표를 따르되,
           - 먼저 "아무 계약도 없는 칸"을 우선순위 순서대로 찾는다.
           - 그런 칸이 없으면 "자기 자신의 계약이 남아 있는 칸"을 우선순위 순서대로 찾는다.
        3) 이동 결과 같은 칸에 여러 명이 모이면 번호가 가장 작은 한 명만 남고 나머지는 탈락한다.
        4) 모든 계약의 남은 시간이 1씩 줄어든다.

      1번 플레이어만 남는 최초 시각을 출력한다. 1000초 안에 그렇게 되지 않으면 -1.

    ■ 자료 구조
        MAP[r][c]  : 그 칸의 계약 주인 번호 (격자 바깥은 WALL)
        time[r][c] : 그 계약이 몇 초 남았는지 (0이면 계약 없음)

      "주인"과 "남은 시간"을 두 배열로 나눠 두는 것이 핵심이다.
      이동 규칙이 "계약 없음(time == 0)" -> "내 계약(MAP == 나)" 두 단계로 나뉘는데,
      이 두 조건이 각각 배열 하나씩과 정확히 대응된다.

      격자 바깥을 WALL로 채워 두면 우선순위를 훑을 때 범위 검사가 필요 없다.
      (WALL 칸은 time이 0이라 첫 단계에서 걸릴 수 있으므로, 벽 검사는 명시적으로 해 준다)

    ■ 방향 우선순위 표
        priority[d][k] : 현재 방향이 d일 때 k번째로 시도할 방향

      현재 방향에 따라 시도 순서가 통째로 달라지기 때문에 2차원 표로 들고 있다.
      이동에 성공하면 그 방향이 새 현재 방향이 된다.

    ■ 충돌 처리 : 임시 맵으로 "먼저 온 사람"을 기억
      한 턴의 이동을 처리하면서 tmpMAP에 "그 칸에 현재 남아 있는 플레이어"를 적어 둔다.

        - 그 칸이 비어 있으면 그냥 들어간다.
        - 이미 누군가 있으면 둘 중 번호가 작은 쪽만 남기고 다른 쪽을 탈락시킨다.

      세 명 이상이 같은 칸에 모여도, 충돌이 일어날 때마다 한 명씩 탈락하므로
      결과적으로 가장 작은 번호 한 명만 남는다.

    ■ 주의할 점
      1) 계약 시간 감소(4단계)는 반드시 이동과 충돌 처리가 모두 끝난 뒤에 한 번만 해야 한다.
      2) 플레이어가 이동할 수 있는 칸은 "계약 없음" 또는 "내 계약"뿐이라,
         남의 계약이 살아 있는 칸으로는 절대 들어가지 않는다.
         그래서 이동 후 MAP을 갱신해도 남의 유효한 계약을 덮어쓸 일이 없다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (20 + 5)
#define WALL (-1)

int T;
int N, M, K;

// MAP[r][c] : 그 칸의 계약 주인 (0이면 주인 없음, 격자 바깥은 WALL)
int MAP[MAX][MAX];

// time[r][c] : 계약이 남은 시간 (0이면 계약이 살아 있지 않다)
int time[MAX][MAX];

// 플레이어 상태
struct PLAYER
{
    int r;                   // 현재 행
    int c;                   // 현재 열
    int dir;                 // 현재 방향
    int priority[5][5];      // priority[현재 방향][k] = k번째로 시도할 방향
    bool dead;               // 탈락했는지
};

typedef struct PLAYER PLAYER;

PLAYER player[MAX * MAX];

// 방향 번호 : 1 위, 2 아래, 3 왼쪽, 4 오른쪽 (0은 더미)
int dr[] = { 0, -1, 1, 0, 0 };
int dc[] = { 0,  0, 0,-1, 1 };

/*
    입력
*/
void input()
{
    scanf("%d %d %d", &N, &M, &K);

    // 격자 안팎을 일단 전부 WALL로 밀어 둔다.
    // 실제 칸은 바로 아래에서 덮어쓰므로, 결과적으로 바깥 테두리만 WALL로 남는다.
    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            MAP[r][c] = WALL;

    // 초기 보드. 0이 아닌 값은 그 자리에 있는 플레이어 번호다.
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
        {
            scanf("%d", &MAP[r][c]);

            int playerNumber = MAP[r][c];
            if (playerNumber != 0)
            {
                player[playerNumber].r = r;
                player[playerNumber].c = c;
            }
        }
    }

    // 초기 방향
    for (int i = 1; i <= M; i++)
    {
        int dir;
        scanf("%d", &dir);
        player[i].dir = dir;
    }

    // 방향 우선순위 표 (현재 방향 d마다 시도 순서 4개)
    for (int i = 1; i <= M; i++)
        for (int d = 1; d <= 4; d++)
            scanf("%d %d %d %d",
                &player[i].priority[d][1],
                &player[i].priority[d][2],
                &player[i].priority[d][3],
                &player[i].priority[d][4]);
}

/*
    디버그용 맵 출력
*/
void printMap(int map[MAX][MAX])
{
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
            printf("%d ", map[r][c]);
        putchar('\n');
    }
    putchar('\n');
}

/*
    디버그용 플레이어 한 명 출력
*/
void printPlayer(int number)
{
    PLAYER p = player[number];
    printf("number %d / (%d, %d) / dir %d / dead %d\n",
        number, p.r, p.c, p.dir, p.dead);
}

/*
    디버그용 전체 플레이어 출력
*/
void printPlayerAll()
{
    for (int m = 1; m <= M; m++)
        printPlayer(m);
}

/*
    이번 턴에 이동할 방향 결정

      1순위 : 계약이 하나도 없는 칸 (time == 0)
      2순위 : 내 계약이 남아 있는 칸 (MAP == 나)

    두 단계 모두 우선순위 표 순서대로 훑는다.
    1순위를 전부 확인한 뒤에 2순위를 보는 것이 중요하다.
    (한 번에 섞어서 보면 "빈 칸이 있는데도 내 구역으로 가는" 잘못된 선택이 나온다)
*/
int getDirection(int number)
{
    PLAYER p = player[number];
    int dir = p.dir;

    // 1단계 : 계약 없는 칸
    for (int i = 1; i <= 4; i++)
    {
        int nextDir = p.priority[dir][i];
        int nr = p.r + dr[nextDir];
        int nc = p.c + dc[nextDir];

        // 격자 바깥은 계약이 없어 보이므로 반드시 따로 걸러야 한다
        if (MAP[nr][nc] == WALL) continue;

        if (time[nr][nc] == 0)
            return nextDir;
    }

    // 2단계 : 내 계약이 살아 있는 칸
    // (여기서는 벽을 따로 안 걸러도 된다. 벽의 MAP 값은 WALL이라 number와 같아질 수 없다)
    for (int i = 1; i <= 4; i++)
    {
        int nextDir = p.priority[dir][i];
        int nr = p.r + dr[nextDir];
        int nc = p.c + dc[nextDir];

        if (MAP[nr][nc] == number)
            return nextDir;
    }

    return -1;   // 자기 계약은 지금 서 있는 칸에도 있으므로 정상적으로는 나오지 않는다
}

/*
    1초씩 최대 1000초까지 시뮬레이션

      반환값 : 1명만 남은 시각. 끝까지 안 끝나면 -1.
*/
int simulate()
{
    int playerCount = M;   // 살아 있는 플레이어 수

    for (int i = 1; i <= 1000; i++)
    {
        // 1) 지금 서 있는 칸에 계약을 새로 남긴다 (남은 시간을 K로 갱신)
        for (int p = 1; p <= M; p++)
        {
            if (player[p].dead == true) continue;

            int r = player[p].r;
            int c = player[p].c;

            MAP[r][c] = p;
            time[r][c] = K;
        }

        // 2) 이동. tmpMAP에는 "이번 턴에 그 칸을 차지한 플레이어"를 기록한다.
        int tmpMAP[MAX][MAX] = { 0 };

        for (int p = 1; p <= M; p++)
        {
            if (player[p].dead == true) continue;

            int sr = player[p].r;
            int sc = player[p].c;

            int dir = getDirection(p);

            int nr = sr + dr[dir];
            int nc = sc + dc[dir];

            // 3) 충돌 처리
            if (tmpMAP[nr][nc] == 0)
            {
                // 아직 아무도 없는 칸 -> 그대로 이동
                player[p].r = nr;
                player[p].c = nc;
                player[p].dir = dir;

                tmpMAP[nr][nc] = p;
            }
            else
            {
                // 이미 누군가 차지한 칸 -> 번호가 작은 쪽만 남는다
                int anotherPlayer = tmpMAP[nr][nc];

                playerCount--;   // 어느 쪽이든 한 명은 탈락한다

                if (anotherPlayer < p)
                {
                    // 먼저 있던 쪽이 더 작다 -> 내가 탈락
                    player[p].dead = true;
                }
                else
                {
                    // 내가 더 작다 -> 있던 쪽을 밀어내고 내가 차지
                    player[anotherPlayer].dead = true;

                    player[p].r = nr;
                    player[p].c = nc;
                    player[p].dir = dir;

                    tmpMAP[nr][nc] = p;
                }
            }
        }

        // 이동 결과를 MAP에 반영한다.
        // 플레이어는 "계약 없는 칸"이나 "자기 계약 칸"으로만 가므로
        // 남의 살아 있는 계약을 덮어쓰는 일은 생기지 않는다.
        for (int r = 1; r <= N; r++)
        {
            for (int c = 1; c <= N; c++)
            {
                if (tmpMAP[r][c] == 0) continue;
                MAP[r][c] = tmpMAP[r][c];
            }
        }

        // 디버그용
        // printf("MAP\n"); printMap(MAP);
        // printf("tmpMAP\n"); printMap(tmpMAP);

        // 한 명만 남았으면 그 시각이 정답이다
        if (playerCount == 1) return i;

        // 4) 계약 시간 감소. 이동과 충돌이 모두 끝난 뒤에 한 번만 한다.
        for (int r = 1; r <= N; r++)
            for (int c = 1; c <= N; c++)
                if (time[r][c] != 0) time[r][c]--;
    }

    return -1;
}

int main()
{
    // 이 문제는 테스트 케이스가 하나다
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();
        printf("%d\n", simulate());
    }

    return 0;
}
