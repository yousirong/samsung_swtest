#include <stdio.h>

#define MAX_N (15+5)
#define MAX_M (30+5)

#define INF (0x7fff0000)
#define BASECAMP (1)
#define WALL (2)

int T;

int N, M;

int MAP[MAX_N][MAX_N];
int BLOCK[MAX_N][MAX_N]; // 이동 불가 확인

struct RC
{
    int r;
    int c;
};

RC STORE[MAX_M];
RC PLAYER[MAX_M];
RC queue[MAX_N * MAX_N];

// ↑, ←, →, ↓
int dr[] = { -1, 0, 0, 1 };
int dc[] = { 0,-1,1,0 };

void input()
{
    scanf("%d %d\n", &N, &M);

    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            MAP[r][c] = BLOCK[r][c] = 0;

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            scanf("%d", &MAP[r][c]);

    for (int m = 1; m <= M; m++)
    {
        int r, c;

        scanf("%d %d", &r, &c);

        STORE[m].r = r;
        STORE[m].c = c;
    }
}

void printStatus() // for debug
{
    int tmpMAP[MAX_N][MAX_N] = { 0 };

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            tmpMAP[r][c] = MAP[r][c];

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            if(tmpMAP[r][c] == BASECAMP) tmpMAP[r][c] =-1;

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            if (tmpMAP[r][c] == WALL) tmpMAP[r][c] = -2;

    for (int p = 1; p <= M; p++)
        tmpMAP[PLAYER[p].r][PLAYER[p].c] = p;

    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
            printf("%2d ", tmpMAP[r][c]);
        putchar('\n');
    }
    putchar('\n');
}

void printBefore(RC before[MAX_N][MAX_N])
{
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
            printf("(%d, %d) ", before[r][c].r, before[r][c].c);
        putchar('\n');
    }
    putchar('\n');

}

RC getNextStep(int index) // BFS
{
    // rp wp 초기화
    // sr sc에 PLAYER 좌표
    // er ec에 STORE 좌표
    // queue에 sr sc 좌표 넣기
    // visit 배열에 1
    // before 배열에 -1
    // while(rp<wp)
    // queue에서 좌표 뽑기
    // br, bc 이전 좌표가 뭔지 역으로 추척

    // 상 좌 우 하 방향 돌면서 벽이랑 방문한거랑 맵 밖에 넘어가는거 제외
    // queue[wp] 돌리기
    // visit배열에 +1
    // before배열에 out좌표 갱신

    RC ret;
    int rp, wp;
    int visit[MAX_N][MAX_N] = { 0 };
    RC before[MAX_N][MAX_N] = { 0 };

    rp = wp = 0;

    int sr = PLAYER[index].r;
    int sc = PLAYER[index].c;
    int er = STORE[index].r;
    int ec = STORE[index].c;

    queue[wp].r = sr;
    queue[wp++].c = sc;

    visit[sr][sc] = 1;

    before[sr][sc].r = -1;
    before[sr][sc].c = -1;

    while (rp < wp)
    {
        RC out = queue[rp++];

        if (er == out.r && ec == out.c)
        {
            int tr = er;
            int tc = ec;
            while (1)
            {
                int br, bc; // 이전 좌표

                br = before[tr][tc].r;
                bc = before[tr][tc].c;

                if (br == sr && bc == sc) break;

                tr = br;
                tc = bc;
            }

            ret.r = tr;
            ret.c = tc;
        }

        for (int i = 0; i < 4; i++)
        {
            int nr, nc;

            nr = out.r + dr[i];
            nc = out.c + dc[i];

            if (nr<1 || nc<1 || nr>N || nc>N) continue;
            if (visit[nr][nc] != 0 || BLOCK[nr][nc] == WALL) continue;

            queue[wp].r = nr;
            queue[wp++].c = nc;

            visit[nr][nc] = visit[out.r][out.c] + 1;

            before[nr][nc] = out;
        }

    }

    //ret.r = ret.c = -1;

    return ret; 
}

RC getBaseCamp(int index) // BFS
{
    // rp wp 초기화
    // sr sc에 PLAYER 좌표
    // er ec에 STORE 좌표
    // queue에 sr sc 좌표 넣기
    // visit 배열에 1
    // before 배열에 -1
    // while(rp<wp)
    // queue에서 좌표 뽑기
    // 상좌우하 도는게 아니고 basecamp 후보 찾는데 mindistance 보다
    // 제일 작은거 찾기 해야함
    // 만약 mindistance가 같을 경우 행이랑 열 둘다 작은거 고르기

    // 상 좌 우 하 방향 돌면서 벽이랑 방문한거랑 맵 밖에 넘어가는거 제외
    // queue[wp] 돌리기
    // visit배열에 +1
    // before배열에 out좌표 갱신
    RC ret;
    int rp, wp;
    int visit[MAX_N][MAX_N] = { 0 };
    RC before[MAX_N][MAX_N] = { 0 };

    rp = wp = 0;

    int sr = STORE[index].r;
    int sc = STORE[index].c;

    queue[wp].r = sr;
    queue[wp++].c = sc;

    visit[sr][sc] = 1;

    ret.r = ret.c = INF;
    int minDistance = INF;

    while (rp < wp)
    {
        RC out = queue[rp++];

        if (MAP[out.r][out.c] == BASECAMP && BLOCK[out.r][out.c]==0)
        {
            if (visit[out.r][out.c] < minDistance)
            {
                minDistance = visit[out.r][out.c];
                ret = out;
            }
            else if (visit[out.r][out.c] == minDistance)
            {
                if (out.r < ret.r) ret = out;
                else if (out.r == ret.r)
                {
                    if (out.c < ret.c)
                        ret = out;
                }
            }
            continue;
        }

        for (int i = 0; i < 4; i++)
        {
            int nr, nc;

            nr = out.r + dr[i];
            nc = out.c + dc[i];

            if (nr<1 || nc<1 || nr>N || nc>N) continue;
            if (visit[nr][nc] != 0 || BLOCK[nr][nc] == WALL) continue;

            queue[wp].r = nr;
            queue[wp++].c = nc;

            visit[nr][nc] = visit[out.r][out.c] + 1;
        }
    }
    return ret;
}

int simulate()
{
    // 1. time이 1일때 
    // 본인이 가고 싶은 편의점 방향을 향해서 1칸 움직임
    // nextstep에 getNextStep함수로결과 받기
    int time = 0;
    while (1)
    {
        RC nextStep[MAX_M] = { 0 };
        for (int p = 1; p <= time; p++)
        {
            if (p > M) break;
            if (PLAYER[p].r == STORE[p].r && PLAYER[p].c == STORE[p].c) continue;

            nextStep[p] = getNextStep(p);
        }
        // 2. time이 1일때
        // 편의점에 도착하면 count 1 증가하고 
        // player 좌표는 해당 좌표로 갱신하고 
        // store의 좌표는 block으로 못가게 해야함.
        // time 1증가
        int count = 0;
        for (int p = 1; p <= time; p++)
        {
            if (p > M) break;
            if (PLAYER[p].r == STORE[p].r && PLAYER[p].c == STORE[p].c)
            {
                count++;
                continue;
            }
            int nr, nc;

            nr = nextStep[p].r;
            nc = nextStep[p].c;

            PLAYER[p].r = nr;
            PLAYER[p].c = nc;

            if (nr == STORE[p].r && nc == STORE[p].c)
                BLOCK[nr][nc] = WALL;
        }
        if (count == M) return time;
        time++;


        // 3. getBaseCamp 함수에 time 넣고 받은 결과를 player[time]
        // 에 저장하기
        if (time <= M)
        {
            RC position = getBaseCamp(time);
            BLOCK[position.r][position.c] = WALL;

            PLAYER[time] = position;
        }
    }

    return -1; // for debug

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