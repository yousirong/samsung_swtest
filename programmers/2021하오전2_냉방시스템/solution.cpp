#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용	

#define MAX (20+10)

#define OFFICE (1)
#define LEFT (2)
#define UP (3)
#define RIGHT (4)
#define DOWN (5)

int T;

int N, K, W;
int MAP[MAX][MAX];
int temperature[MAX][MAX];

struct RC
{
	int r;
	int c;
};

RC office[MAX * MAX];
int ocnt;

struct RCD
{
	int r;
	int c;
	int dir;
};

RCD airConditioner[MAX * MAX];
int acnt;

struct RCL
{
	int r;
	int c;
	int length;
};

RCL queue[MAX * MAX];

struct WALL
{
	bool direction[6];
};

WALL wall[MAX][MAX];

// -, -, ←, ↑, →, ↓
int dr[] = { 0,0,0,-1,0,1 };
int dc[] = { 0,0,-1,0,1,0 };

// [수정] scanf 대신 인자로 받는다. walls[i] = {r, c, s}
void input(int k, const std::vector<std::vector<int>>& board, const std::vector<std::vector<int>>& walls)
{
	N = (int)board.size();    // [수정] scanf 대체
	W = (int)walls.size();    // [수정] scanf 대체
	K = k;                    // [수정] scanf 대체

	// [추가] 재호출 대비.
	//        원본은 wall 에 true 만 찍고 temperature 도 되돌리지 않아,
	//        같은 프로세스에서 두 번째 호출하면 이전 벽과 온도가 그대로 남는다.
	for (int r = 0; r <= N + 1; r++)
	{
		for (int c = 0; c <= N + 1; c++)
		{
			temperature[r][c] = 0;
			for (int d = 0; d < 6; d++) wall[r][c].direction[d] = false;
		}
	}

	ocnt = acnt = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			MAP[r][c] = board[r - 1][c - 1];   // [수정] scanf 대체

			if (MAP[r][c] == OFFICE)
			{
				office[ocnt].r = r;
				office[ocnt++].c = c;
			}
			else if (MAP[r][c] != 0)
			{
				airConditioner[acnt].r = r;
				airConditioner[acnt].c = c;
				airConditioner[acnt++].dir = MAP[r][c];
			}
		}
	}

	for (int w = 0; w < W; w++)
	{
		int r, c, s;

		r = walls[w][0];   // [수정] scanf 대체
		c = walls[w][1];   // [수정] scanf 대체
		s = walls[w][2];   // [수정] scanf 대체

		if (s == 0)
		{
			wall[r][c].direction[UP] = true;
			wall[r - 1][c].direction[DOWN] = true;
		}
		else // s ==1
		{
			wall[r][c].direction[LEFT] = true;
			wall[r][c-1].direction[RIGHT] = true;
		}
	}
}

void printMap(int map[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void BFS(int r, int c, int dir)
{
	int rp, wp;
	bool visit[MAX][MAX] = { 0 };
	int sr, sc;

	sr = r + dr[dir];
	sc = c + dc[dir];

	rp = wp = 0;

	queue[wp].r = sr;
	queue[wp].c = sc;
	queue[wp++].length = 5;

	visit[sr][sc] = true;

	while (rp < wp)
	{
		RCL out = queue[rp++];

		if (out.length == 0) break;

		if (out.r<1 || out.c<1 || out.r >N || out.c >N) continue;

		temperature[out.r][out.c] += out.length;

		if (dir == RIGHT || dir == LEFT)
		{
			int nr, nc;

			nc = out.c + dc[dir];

			// ↖ ↗ 위
			nr = out.r - 1;
			if (visit[nr][nc] == false
				//(r,c) 와 (r-1,c) 사이에 벽이 없어야 한다.
				&& (wall[out.r][out.c].direction[UP] == false)
				//(r-1,c) 와 (r-1,c + dc[dir]) 사이에 벽이 없어야 한다.
				&& (wall[nr][out.c].direction[dir] == false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;

				visit[nr][nc] = true;
			}

			// ← → 옆
			nr = out.r;
			if (visit[nr][nc] == false
				// (r,c)와 (r, c+dc[dir]) 사이에 벽이 없어야 한다.
				&& (wall[out.r][out.c].direction[dir] = false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;
				
				visit[nr][nc] = true;
			}
				
			// ↙ ↘ 아래
			nr = out.r + 1;
			if (visit[nr][nc] == false
				// (r, c)와 (r + 1, c) 사이에 벽이 없어야 한다.
				&& (wall[out.r][out.c].direction[DOWN] == false)
				// (r + 1, c)와 (r + 1, c + dc[dir]) 사이에 벽이 없어야 한다.
				&& (wall[nr][out.c].direction[dir] == false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;

				visit[nr][nc] = true;
			}
		}
		else // UP or DOWN
		{
			int nr, nc;

			nr = out.r + dr[dir];

			// ↖ ↙ 왼
			nc = out.c - 1;
			if (visit[nr][nc] == false
				// (r, c)와 (r, c - 1) 사이에 벽이 없어야 한다.
				&& (wall[out.r][out.c].direction[LEFT] == false)
				// (r, c - 1)와 (r + dr[dir], c - 1) 사이에 벽이 없어야 한다.
				&& (wall[out.r][nc].direction[dir] == false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;

				visit[nr][nc] = true;
			}

			// ↑ ↓ 위, 아래
			nc = out.c;
			if (visit[nr][nc] == false
				// (r, c)와 (r + dr[dir], c) 사이에 벽이 없어야 한다.
				&& (wall[out.r][out.c].direction[dir] == false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;

				visit[nr][nc] = true;
			}

			// ↗ ↘
			nc = out.c + 1;
			if (visit[nr][nc] == false
				// (r, c)와 (r, c + 1) 사이에 벽이 없어야 한다.
				&& (wall[out.r][out.c].direction[RIGHT] == false)
				// (r, c + 1)와 (r + dr[dir], c + 1) 사이에 벽이 없어야 한다.
				&& (wall[out.r][nc].direction[dir] == false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;

				visit[nr][nc] = true;
			}
		}
	}
}
void controlTemperature()
{
	int tmpTemp[MAX][MAX] = { 0 };

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (temperature[r][c] == 0) continue;

			int value = temperature[r][c];
			for (int i = 2; i <= 5; i++)
			{
				int nr, nc;

				nr = r + dr[i];
				nc = c + dc[i];

				if (nr <1 || nc <1 || nr>N || nc > N) continue;
				if (wall[r][c].direction[i] == true) continue;

				if (temperature[r][c] > temperature[nr][nc])
				{
					int diff = (temperature[r][c] - temperature[nr][nc]) / 4;

					value -= diff;
					tmpTemp[nr][nc] += diff;
				}
			}

			tmpTemp[r][c] += value;
		}
	}

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			temperature[r][c] = tmpTemp[r][c];
}

void decreaseTemperature()
{
	if (temperature[1][1] != 0) temperature[1][1]--;
	if (temperature[N][1] != 0) temperature[N][1]--;
	if (temperature[1][N] != 0) temperature[1][N]--;
	if (temperature[N][N] != 0) temperature[N][N]--;

	for (int r = 2; r <= N - 1; r++)
		if (temperature[r][1] != 0) temperature[r][1]--;

	for (int r = 2; r <= N - 1; r++)
		if (temperature[r][N] != 0) temperature[r][N]--;

	for (int c = 2; c <= N - 1; c++)
		if (temperature[1][c] != 0)temperature[1][c]--;

	for (int c = 2; c <= N - 1; c++)
		if (temperature[N][c] != 0)temperature[N][c]--;

}

bool checkOffice()
{
	for (int i = 0; i < ocnt; i++)
		if (temperature[office[i].r][office[i].c] < K)
			return false;

	return true;

}

int simulate()
{
	int time = 0;

	while (1)
	{
		for (int i = 0; i < acnt; i++)
		{
			int r, c, dir;

			r = airConditioner[i].r;
			c = airConditioner[i].c;
			dir = airConditioner[i].dir;

			BFS(r, c, dir);
		}
		controlTemperature();

		decreaseTemperature();

		time++;
		if (time > 100) return -1;

		if (checkOffice() == true) break;
	}

	return time;
}


// [수정] main() -> solution().  T 루프 껍데기는 제거했다.
int solution(int k, std::vector<std::vector<int>> board, std::vector<std::vector<int>> walls)
{
	input(k, board, walls);

	return simulate();   // [수정] printf -> return
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	int n, w, k;
	scanf("%d %d %d", &n, &w, &k);   // 원본 scanf 순서 그대로
	std::vector<std::vector<int>> board(n, std::vector<int>(n));
	for (int r = 0; r < n; r++) for (int c = 0; c < n; c++) scanf("%d", &board[r][c]);
	std::vector<std::vector<int>> walls(w, std::vector<int>(3));
	for (int i = 0; i < w; i++) scanf("%d %d %d", &walls[i][0], &walls[i][1], &walls[i][2]);

	int ans = solution(k, board, walls);
#ifdef REPEAT_TEST
	int ans2 = solution(k, board, walls);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif
	printf("%d\n", ans);
	return 0;
}
#endif
