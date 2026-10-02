#include <stdio.h>    

#define MAX (20+5)
#define CORAL (1)

int T;
int N, M, K;

int MAP[MAX][MAX];
int P[MAX][MAX]; // 분출 임계치 P
int current[MAX][MAX]; // 현재 압력

// 우하좌상
int dr[] = { 0,1,0,-1 };
int dc[] = { 1,0,-1,0 };

struct RC
{
    int r;
    int c;
};

RC queue[MAX * MAX];
int rp, wp;

struct RCC
{
    int r;
    int c;
    int check;
    int count; // 거북이 턴 번호
};

RCC volcano[1000 + 10];
RCC turtle[10 + 5];
int turtleMAP[MAX][MAX];

void input()
{
    scanf("%d %d %d", &N, &M, &K);

    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            scanf("%d", &MAP[r][c]);

    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            turtleMAP[r][c] = 0;

    for (int m = 1; m <= M; m++) // ID가 1번부터 시작
    {
        int r, c;

        scanf("%d %d", &r, &c);

        turtle[m].r = r;
        turtle[m].c = c;
        turtle[m].check = 0;
        turtle[m].count = 0;

        turtleMAP[r][c] = m;
    }

    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            current[r][c] = 0;

    for (int k = 0; k < K; k++)
    {
        int r, c, p;

        scanf("%d %d %d", &r, &c, &p);

        P[r][c] = p;
        volcano[k].r = r;
        volcano[k].c = c;
    }
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

RC getNextStep(int index)    // BFS
{
    RC ret;
    int rp, wp;
    int visit[MAX][MAX] = { 0 };
    RC before[MAX][MAX] = { 0 };

    rp = wp = 0;

    int sr = turtle[index].r;
    int sc = turtle[index].c;
    int er = N - 1;
    int ec = N - 1;

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

            return ret;
        }

        for (int i = 0; i < 4; i++)
        {
            int nr, nc;

            nr = out.r + dr[i];
            nc = out.c + dc[i];

            if (nr<0 || nc<0 || nr>N - 1 || nc>N - 1) continue;
            if (visit[nr][nc] != 0 || MAP[nr][nc] != 0 || turtleMAP[nr][nc] != 0)continue;

            queue[wp].r = nr;
            queue[wp++].c = nc;

            visit[nr][nc] = visit[out.r][out.c] + 1;

            before[nr][nc] = out;
        }
    }

    ret.r = ret.c = -1;

    return ret; // for debug
}

void move()
{
    for (int m = 1; m <= M; m++)
    {
        if (turtle[m].check == -1) continue;

        turtle[m].count++;

        RC next = getNextStep(m);

        if (next.r == -1) continue;

        int r, c, nr, nc;

        r = turtle[m].r;
        c = turtle[m].c;
        nr = next.r;
        nc = next.c;

        turtleMAP[r][c] = 0;

        turtle[m].r = nr;
        turtle[m].c = nc;

        turtleMAP[nr][nc] = m;

        if (nr == N - 1 && nc == N - 1)
        {
            turtle[m].check = -1;
            turtleMAP[nr][nc] = 0;
        }
    }
}

void increase()
{
    for (int k = 0; k < K; k++)
    {
        int r, c;

        r = volcano[k].r;
        c = volcano[k].c;

        current[r][c] += 10;
    }
}

void explode()
{
    int heat[MAX][MAX] = { 0 };

    // 열기 전파
    for (int k = 0; k < K; k++)
    {
        int r, c;

        r = volcano[k].r;
        c = volcano[k].c;

        if (current[r][c] >= P[r][c])
        {
            volcano[k].check = 1;
            heat[r][c] += P[r][c];

            for (int i = 0; i < 4; i++)
            {
                int nr, nc;

                nr = r + dr[i];
                nc = c + dc[i];

                int p = P[r][c] / 2;

                while (1)
                {
                    if (nr<0 || nc<0 || nr>N - 1 || nc>N - 1) break;
                    if (MAP[nr][nc] == CORAL || p == 0) break;

                    heat[nr][nc] += p;

                    p = p / 2;
                    nr += dr[i];
                    nc += dc[i];
                }
            }
        }
    }

    // 연쇄 반응
    while (1)
    {
        bool explodeCheck = false;

        for (int k = 0; k < K; k++)
        {
            if (volcano[k].check == 1) continue;

            int r, c;

            r = volcano[k].r;
            c = volcano[k].c;

            if (current[r][c] + heat[r][c] >= P[r][c])
            {
                explodeCheck = true;

                volcano[k].check = 1;
                heat[r][c] += P[r][c];

                for (int i = 0; i < 4; i++)
                {
                    int nr, nc;

                    nr = r + dr[i];
                    nc = c + dc[i];

                    int p = P[r][c] / 2;

                    while (1)
                    {
                        if (nr<0 || nc<0 || nr>N - 1 || nc>N - 1) break;
                        if (MAP[nr][nc] == CORAL || p == 0)break;

                        heat[nr][nc] += p;

                        p = p / 2;
                        nr += dr[i];
                        nc += dc[i];
                    }
                }
            }
        }
        if (explodeCheck == false) break;
    }

    // 바다거북의 위기 (화석화)
    for (int m = 1; m <= M; m++)
    {
        if (turtle[m].check == -1)continue;

        int r, c;

        r = turtle[m].r;
        c = turtle[m].c;

        if (heat[r][c] >= 20)
        {
            turtle[m].check = -1;
            turtle[m].count = -1;
        }
    }

}

void reset()
{
    for (int k = 0; k < K; k++)
    {
        int r, c;

        r = volcano[k].r;
        c = volcano[k].c;

        if (volcano[k].check == 1) current[r][c] = 0;

        volcano[k].check = 0;
    }
}

void simulate()
{
    for (int i = 0; i < 100; i++)
    {
        move();
        increase();
        explode();
        reset();
    }

    for (int m = 1; m <= M; m++)
    {
        //도착실패
        if (turtle[m].check == 0) printf("-1\n");
        else printf("%d\n", turtle[m].count);
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