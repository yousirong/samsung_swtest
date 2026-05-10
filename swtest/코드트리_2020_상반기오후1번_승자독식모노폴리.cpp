// 코드트리 2020 상반기 오후 1번 - 승자독식 모노폴리
//
// 문제 요약
// N x N 격자 위에 M명의 플레이어가 있고,
// 각 플레이어는 현재 위치, 방향, 방향 우선순위를 가진다.
//
// 모든 플레이어는 자신이 지나간 칸에 "독점 계약"을 남기며,
// 그 계약은 K초 동안 유지된다.
//
// 매 초마다 진행 규칙
// 1. 살아 있는 모든 플레이어가 현재 칸에 자신의 독점 계약을 남긴다.
// 2. 각 플레이어는 자신의 현재 방향에 따른 우선순위를 기준으로 이동한다.
//    - 먼저 독점 계약이 없는 빈 칸을 찾는다.
//    - 없으면 자기 자신의 독점 계약이 남아 있는 칸으로 이동한다.
// 3. 여러 플레이어가 같은 칸에 도착하면 번호가 작은 플레이어만 살아남는다.
// 4. 모든 독점 계약의 남은 시간을 1씩 감소시킨다.
//
// 목표
// 1번 플레이어만 남게 되는 최초 시간을 구한다.
// 1000초 안에 불가능하면 -1을 출력한다.
//
// 핵심 아이디어
// - MAP[r][c]  : 현재 그 칸을 독점하고 있는 플레이어 번호
// - time[r][c] : 해당 독점 계약이 몇 초 남았는지
// - player[i]  : i번 플레이어의 위치, 방향, 우선순위, 생존 여부
// - getDirection() : 플레이어가 다음에 이동할 방향 결정
// - simulate()     : 전체 과정을 1초씩 시뮬레이션
//
// 즉,
// "독점 계약 갱신 -> 플레이어 이동 -> 충돌 처리 -> 계약 시간 감소"
// 를 반복하는 시뮬레이션 문제이다.

#include <stdio.h>
#include <stdbool.h>   // C에서 bool, true, false 사용하려면 필요

#define MAX (20 + 5)
#define WALL (-1)

int T;
int N, M, K;

// MAP[r][c] = 현재 (r,c) 칸을 독점하고 있는 플레이어 번호
// 빈칸은 0, 바깥 경계는 WALL
int MAP[MAX][MAX];

// time[r][c] = (r,c) 칸의 독점 계약이 남아 있는 시간
int time[MAX][MAX];

// 플레이어 상태 구조체
struct PLAYER
{
    int r;                   // 현재 행
    int c;                   // 현재 열
    int dir;                 // 현재 방향
    int priority[5][5];      // priority[현재방향][우선순위 번호] = 다음으로 시도할 방향
    bool dead;               // 죽었는지 여부
};

typedef struct PLAYER PLAYER;

// player[i] = i번 플레이어 정보
PLAYER player[MAX * MAX];

// 방향 번호
// -, 위, 아래, 왼쪽, 오른쪽
int dr[] = { 0, -1, 1, 0, 0 };
int dc[] = { 0,  0, 0,-1, 1 };

/*
    입력 함수

    N : 보드 크기
    M : 플레이어 수
    K : 독점 계약 유지 시간

    입력 순서
    1. 초기 보드 상태
    2. 각 플레이어의 초기 방향
    3. 각 플레이어의 방향 우선순위
*/
void input()
{
    scanf("%d %d %d", &N, &M, &K);

    // 전체 맵을 WALL로 초기화해서 경계 처리를 쉽게 한다.
    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            MAP[r][c] = WALL;

    // 실제 보드 입력
    // MAP[r][c]에 플레이어 번호가 들어 있으면 해당 플레이어의 시작 위치 저장
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

    // 각 플레이어의 초기 방향 입력
    for (int i = 1; i <= M; i++)
    {
        int dir;
        scanf("%d", &dir);
        player[i].dir = dir;
    }

    // 각 플레이어의 방향 우선순위 입력
    // priority[d][1..4] :
    // 현재 방향이 d일 때, 어떤 방향 순서로 탐색할지 저장
    for (int i = 1; i <= M; i++)
        for (int d = 1; d <= 4; d++)
            scanf("%d %d %d %d",
                &player[i].priority[d][1],
                &player[i].priority[d][2],
                &player[i].priority[d][3],
                &player[i].priority[d][4]);
}

/*
    디버그용 맵 출력 함수
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
    디버그용 플레이어 1명 출력
*/
void printPlayer(int number)
{
    PLAYER p = player[number];
    printf("number %d / (%d, %d) / dir %d / dead %d\n",
        number, p.r, p.c, p.dir, p.dead);
}

/*
    디버그용 모든 플레이어 출력
*/
void printPlayerAll()
{
    for (int m = 1; m <= M; m++)
        printPlayer(m);
}

/*
    다음 이동 방향을 결정하는 함수

    규칙
    1. 현재 방향 기준 우선순위대로 보면서
       "독점 계약이 없는 빈 칸(time == 0)"을 먼저 찾는다.
    2. 그런 칸이 없으면
       다시 우선순위대로 보면서 "자기 자신의 독점 구역(MAP == number)"을 찾는다.

    반환값
    - 이동할 방향 번호
    - 없으면 -1 (디버그용 안전장치)
*/
int getDirection(int number)
{
    PLAYER p = player[number];
    int dir = p.dir;

    // -------------------------
    // 1. 독점 계약이 없는 빈 공간 찾기
    // -------------------------
    for (int i = 1; i <= 4; i++)
    {
        int nextDir = p.priority[dir][i];   // 우선순위에 따른 다음 방향
        int nr = p.r + dr[nextDir];
        int nc = p.c + dc[nextDir];

        // 벽이면 제외
        if (MAP[nr][nc] == WALL) continue;

        // 독점 계약이 전혀 없는 칸이면 그 방향 선택
        if (time[nr][nc] == 0)
            return nextDir;
    }

    // -------------------------
    // 2. 자신의 독점 구역 찾기
    // -------------------------
    for (int i = 1; i <= 4; i++)
    {
        int nextDir = p.priority[dir][i];
        int nr = p.r + dr[nextDir];
        int nc = p.c + dc[nextDir];

        // MAP[nr][nc] == number 라는 것은
        // 현재 그 칸이 자기 플레이어의 독점 구역이라는 뜻
        if (MAP[nr][nc] == number)
            return nextDir;
    }

    return -1;   // 정상 상황에서는 거의 나오지 않음
}

/*
    전체 시뮬레이션 함수

    최대 1000초 동안 다음 과정을 반복한다.
    1. 살아 있는 플레이어들이 현재 칸에 독점 계약을 남김
    2. 각 플레이어 이동
    3. 같은 칸 충돌 시 번호가 작은 플레이어만 생존
    4. 플레이어가 1명만 남으면 시간 반환
    5. 모든 독점 계약 시간 1 감소

    반환값
    - 1번 플레이어만 남게 된 최초 시간
    - 1000초 안에 불가능하면 -1
*/
int simulate()
{
    int playerCount = M;   // 현재 살아 있는 플레이어 수

    for (int i = 1; i <= 1000; i++)
    {
        // -------------------------
        // 1. 현재 위치에 독점 계약 남기기
        // -------------------------
        for (int p = 1; p <= M; p++)
        {
            if (player[p].dead == true) continue;

            int r = player[p].r;
            int c = player[p].c;

            MAP[r][c] = p;
            time[r][c] = K;
        }

        // -------------------------
        // 2. 이번 턴 이동 결과를 임시 맵에 저장
        // -------------------------
        // tmpMAP[r][c] = 이번 초 이동이 끝난 뒤
        //                최종적으로 그 칸에 남는 플레이어 번호
        int tmpMAP[MAX][MAX] = { 0 };

        for (int p = 1; p <= M; p++)
        {
            if (player[p].dead == true) continue;

            int sr = player[p].r;
            int sc = player[p].c;

            int dir = getDirection(p);

            int nr = sr + dr[dir];
            int nc = sc + dc[dir];

            // -------------------------
            // 3. 이동 후 충돌 처리
            // -------------------------
            if (tmpMAP[nr][nc] == 0)
            {
                // 아직 아무도 없는 칸이면 그대로 이동
                player[p].r = nr;
                player[p].c = nc;
                player[p].dir = dir;

                tmpMAP[nr][nc] = p;
            }
            else
            {
                // 이미 다른 플레이어가 먼저 도착한 칸
                int anotherPlayer = tmpMAP[nr][nc];

                // 한 명 탈락하므로 생존자 수 감소
                playerCount--;

                if (anotherPlayer < p)
                {
                    // 기존 플레이어 번호가 더 작으면 현재 플레이어 사망
                    player[p].dead = true;
                }
                else
                {
                    // 현재 플레이어 번호가 더 작으면 기존 플레이어 사망
                    player[anotherPlayer].dead = true;

                    player[p].r = nr;
                    player[p].c = nc;
                    player[p].dir = dir;

                    tmpMAP[nr][nc] = p;
                }
            }
        }

        // -------------------------
        // 4. tmpMAP 결과를 MAP에 반영
        // -------------------------
        // 주의:
        // MAP는 "현재 독점 구역 주인" 개념으로도 쓰이고 있으므로
        // 이동한 플레이어들의 위치를 다시 반영해 준다.
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

        // 플레이어가 1명만 남았다면 현재 시간 반환
        if (playerCount == 1) return i;

        // -------------------------
        // 5. 독점 계약 시간 감소
        // -------------------------
        for (int r = 1; r <= N; r++)
            for (int c = 1; c <= N; c++)
                if (time[r][c] != 0) time[r][c]--;
    }

    return -1;
}

int main()
{
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();
        printf("%d\n", simulate());
    }

    return 0;
}