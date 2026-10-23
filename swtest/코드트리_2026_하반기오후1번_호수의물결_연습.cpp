//

#include <stdio.h>

#define MAX (30+5)
#define MAX_K (50+5)
#define MAX_STONE (500+20)
#define LAST_TURN (50)

int T;
int N, M, K;

struct CATAPULT
{
	int c; // 열
	int s; // 시작 시각
	int d; // 간격
	int a; // 무게
	int h; // 거리
	int p; // 돌 수
};

CATAPULT cat[MAX_K];

struct STONE
{
	int r, c, a; // 지금위치
	int targetRow; // 떨어질 행
	int alive; // 1 = 날고 있음
};

STONE stone[MAX_STONE];
int scnt;

int wave[MAX][MAX]; // 이번턴 물결 세기
int murk[MAX][MAX]; //혼탁도

int score[LAST_TURN + 5];
int turnCount;

struct RC
{
	int r;
	int c;
};

RC queue[MAX * MAX];
int visit[MAX][MAX];

int dr[] = { -1,1,0,0 };
int dc[] = { 0,0,-1,1 };

void input()
{
	scanf("%d %d %d", &N, &M, &K);

	for (int i = 0; i < K; i++)
		scanf("%d %d %d %d %d", % cat[i].c, &cat[i].s, % cat[i].d, % cat[i].a, % cat[i].h, % cat[i].p);

	for (int r = 0; r < N; r++)
		for (int c = 0; c < M; c++)
			murk[r][c] = 0;

	scnt = 0;
	turnCount = 0;	
}

int abs(int x)
{
	return x > 0 ? x : -x;
}

void moveStones()
{
	for (int i = 0; i < scnt; i++)
		if (stone[i].alive) stone[i].r--;
}

void launchStones(int t)
{
	for (int i = 0; i < K; i++)
	{
		if (t < cat[i].s) continue;
		if ((t - cat[i].s) % cat[i].d != 0) continue;
		if ((t - cat[i].s) / cat[i].d >= cat[i].p) continue;

		stone[scnt].r = N - 1;
		stone[scnt].c = cat[i].c;
		stone[scnt].a = cat[i].a;
		stone[scnt].targetRow = N - cat[i].h;
		stone[scnt++].alive = 1;
	}
}

int dropStones()
{
	int dropCount = 0;

	for (int r = 0; r < N; r++)
		for (int c = 0; c < M; c++)
			wave[r][c] = 0;

	for (int i = 0; i < scnt; i++)
	{
		if (stone[i].alive == 0 || stone[i].r != stone[i].targetRow)continue;

		stone[i].alive = 0;
		dropCount++;

		for (int y = 0; y < M; y++)
		{
			for (int x = 0; x < M; x++)
			{
				int power = 3 * stone[i].a - 5 * (abs(y - stone[i].r) + abs(x - stone[i].c));

				if (power > 0) wave[y][x] += power;
			}
		}
	}

	return dropCount;
}

int getScore()
{
	int sum = 0;
	
	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < M; c++)
		{
			int v = wave[r][c] - murk[r][c];
			if (v > 0) sum += v;
		}
	}
	return sum;
}


void settle()
{
	int next[MAX][MAX];

	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < M; c++)
		{
			next[r][c] = murk[r][c];
			if (murk[r][c] == 0) continue;

			int surrounded = 1;
			for (int i = 0; i < 4; i++)
			{
				int nr = r + dr[i];
				int nc = c + dc[i];

				if (nr < 0 || nc < 0 || nr >= N || nr >= N || murk[nr][nc] == 0)
				{
					surrounded = 0;
					break;
				}
			}

			next[r][c] -= surrounded ? 1 : 2;
			if (next[r][c] < 0) next[r][c] = 0;
		}
	}

	for (int r = 0; r < N; r++)
		for (int c = 0; c < M; c++)
			murk[r][c] = next[r][c];
}

void flow()
{
	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < M - 1; c++)
			murk[r][c] = murk[r][c + 1];
		murk[r][M - 1] = 0;
	}
}

void stirUp()
{
	for (int r = 0; r < N; r++)
		for (int c = 0; c < M; c++)
			visit[r][c] = 0;

	for (int sr = 0; sr < N; sr++)
	{
		for (int sc = 0; sc < M; sc++)
		{
			if (wave[sr][sc] <= 0 || visit[sr][sc] == 1) continue;
			
			int rp = 0, wp = 0;
			int sum = 0;

			queue[wp].r = sr;
			queue[wp++].c = sc;
			visit[sr][sc] = 1;

			while (rp < wp)
			{
				RC out = queue[rp++];
				sum += wave[out.r][out.c];

				for (int i = 0; i < 4; i++)
				{
					int nr = out.r + dr[i];
					int nc = out.c + dc[i];

					if (nr < 0 || nc < 0 || nr >= N || nc >= M)continue;
					if (wave[nr][nc] <= 0 || visit[nr][nc] == 1) continue;

					visit[nr][nc] = 1;
					queue[wp].r = nr;
					queue[wp++].c = nc;
				}
			}

			int avg = sum / wp;
			for (int i = 0; i < wp; i++)
				murk[queue[i].r][queue[i].c] += avg;
		}
	}
}

int isRemain(int t)
{
	for (int i = 0; i < scnt; i++)
		if (stone[i].alive) return 1;

	for (int i = 0; i < K; i++)
	{
		int last = cat[i].s + (cat[i].p - 1) * cat[i].d;
		if (last > t) return 1;
	}
	return 0;
}

void simulate()
{
	for (int t = 1; t <= LAST_TURN; t++)
	{
		moveStones(); //1
		launchStones(t); //2
		int dropCount = dropStones(); //3
		score[turnCount++] = (dropCount > 0) ? getScore() : 0; //4
		
		settle(); //5
		flow(); //6
		stirUp(); //7

		if (isRemain(t) == 0) break;

	}
}

int main()
{
	scanf("%d", &T);
	for (int tc = 1; tc <= T; tc++)
	{
		input();
		simulate();
		printf("#%d", tc);
		for (int i = 0; i < turnCount; i++)
			printf(" %d", score[i]);
		putchar('\n');
	}
	return 0;
}