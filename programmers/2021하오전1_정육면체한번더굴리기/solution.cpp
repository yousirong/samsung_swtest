#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용	

#define MAX (20 +5)

#define EAST (1)
#define WEST (2)
#define NORTH (3)
#define SOUTH (4)

int T;
int N, M;
int MAP[MAX][MAX];

bool visit[MAX][MAX];
int scoreBoard[MAX][MAX];

struct CUBE
{
	int up;
	int left; int top; int right;
	int down;
	int bottom;
};

CUBE cube;

struct RC
{
	int r;
	int c;
};

RC queue[MAX * MAX];

// -, 동, 서, 남, 북
int dr[] = { 0,0,0,-1,1 };
int dc[] = { 0,1,-1,0,0 };

// [수정] scanf 대신 인자로 받는다. M은 격자 크기가 아니라 "이동 횟수"다.
void input(int m, const std::vector<std::vector<int>>& board)
{
	N = (int)board.size();   // [수정] scanf 대체
	M = m;                   // [수정] scanf 대체

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] = board[r - 1][c - 1];   // [수정] scanf 대체
}

void printMap(int map[MAX][MAX]) // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void printCube() // for debug
{
	printf("   %d\n", cube.up);
	printf("%d %d %d\n", cube.left, cube.top, cube.right);
	printf("   %d\n", cube.down);
	printf("   %d\n", cube.bottom);
}

void moveEast()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };

	cube.top = tmp[1];
	cube.right = tmp[2];
	cube.bottom = tmp[3];
	cube.left = tmp[5];
}

void moveWest()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };

	cube.top = tmp[3];
	cube.right = tmp[5];
	cube.bottom = tmp[1];
	cube.left = tmp[2];
}

void moveNorth()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };

	cube.up = tmp[2];
	cube.top = tmp[4];
	cube.down = tmp[5];
	cube.bottom = tmp[0];
}

void moveSouth()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };

	cube.up = tmp[5];
	cube.top = tmp[0];
	cube.down = tmp[2];
	cube.bottom = tmp[4];
}

void BFS(int r, int c)
{
	int number;
	int rp, wp;

	number = MAP[r][c];

	rp = wp = 0;

	queue[wp].r = r;
	queue[wp++].c = c;

	visit[r][c] = true;

	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 1; i <= 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			if (nr <1 || nc <1 || nr >N || nc > N) continue;

			if (MAP[nr][nc] != number || visit[nr][nc] == true) continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = true;
		}
	}

	for (int i = 0; i < wp; i++)
	{
		int r, c;

		r = queue[i].r;
		c = queue[i].c;

		scoreBoard[r][c] = number * wp;
	}
}

void makeScoreBoard()
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			visit[r][c] = false;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (visit[r][c] == true) continue;

			BFS(r, c);
		}
	}
	//printMap(scoreBoard);
}

int simulate()
{
	// 동 : 1, 서 : 2, 북 : 3, 남 : 4
	int changeDir[5] = { 0, 2, 1, 4, 3 };
	int changeClock[5] = { 0, 4, 3, 1, 2 };
	int changeCounterClock[5] = { 0, 3, 4, 2, 1 };

	// 방향 전환 배열 만들기
	// changeDir[EAST] = WEST;
	// changeDir[WEST] = EAST;
	// changeDir[NORTH] = SOUTH;
	// changeDir[SOUTH] = NORTH;

	// changeClock[EAST] = SOUTH;
	// changeClock[WEST] = NORTH;
	// changeClock[NORTH] = EAST;
	// changeClock[SOUTH] = WEST;

	// changeCounterClock[EAST] = NORTH;
	// changeCounterClock[WEST] = SOUTH;
	// changeCounterClock[NORTH] = WEST;
	// changeCounterClock[SOUTH] = EAST;

	int sr, sc, dir;

	sr = sc = 1;
	dir = EAST;

	cube.up = 5;
	cube.left = 4; cube.top = 1; cube.right = 3;
	cube.down = 2;
	cube.bottom = 6;

	int score = 0;

	for (int i = 0; i < M; i++)
	{
		int nr, nc;

		nr = sr + dr[dir];
		nc = sc + dc[dir];

		if (nr<1 || nc<1 || nr>N || nc> N)
		{
			dir = changeDir[dir];
			nr = sr + dr[dir];
			nc = sc + dc[dir];
		}

		if (dir == EAST) moveEast();
		else if (dir == WEST) moveEast();
		else if (dir == NORTH) moveNorth();
		else if (dir == SOUTH) moveSouth();

		score += scoreBoard[nr][nc];

		int A = cube.bottom;
		int B = MAP[nr][nc];

		if (A > B) dir = changeClock[dir];
		else if (A < B) dir = changeCounterClock[dir];

		sr = nr;
		sc = nc;
	}
	return score;
}

// [수정] main() -> solution().  T 루프 껍데기는 제거했다.
int solution(int m, std::vector<std::vector<int>> board)
{
	input(m, board);

	makeScoreBoard();

	return simulate();   // [수정] printf -> return
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
//   빌드      : g++ -O2 -DLOCAL_TEST -o run solution.cpp
//   재호출 검사 : g++ -O2 -DLOCAL_TEST -DREPEAT_TEST -o rep solution.cpp
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	int n, x;
	scanf("%d %d", &n, &x);          // 원본 scanf 순서 그대로

	std::vector<std::vector<int>> board(n, std::vector<int>(n));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			scanf("%d", &board[r][c]);

	int ans = solution(x, board);
#ifdef REPEAT_TEST
	int ans2 = solution(x, board);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif
	printf("%d\n", ans);
	return 0;
}
#endif
