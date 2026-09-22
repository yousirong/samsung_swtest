/*
	[코드트리] 2021 하반기 오전 1번 - 정육면체 한번 더 굴리기
	원본 : swtest/코드트리_2021_하반기오전1번_정육면제한번더굴리기.cpp
	[프로그래머스 함수형 사본]  main() 대신 solution()이 값을 받고 돌려준다.
	원본 : swtest/ 아래 같은 이름의 파일. 로직은 그대로 두고 입출력 껍데기만 바꿨다.
	[버그수정] 원본에 있던 버그는 이 사본에서 고쳐 옮겼다. 고친 자리마다 // [버그수정] 표시가 있다.

	https://www.codetree.ai/ko/frequent-problems/samsung-sw/problems/cube-rounding-again/description
	(백준 23288 "주사위 굴리기 2"와 같은 문제다)

	■ 문제 요약
	  N x N 격자의 각 칸에 1 ~ 6 숫자가 있고, (1, 1)에 정육면체(주사위)가 놓여 있다.
	  처음 상태는 윗면 1, 남쪽 면 2, 동쪽 면 3 (마주 보는 면의 합은 7)이고 오른쪽(동)을 향한다.
	  M번 아래를 반복한다.

	    1) 이동 : 보는 방향으로 한 칸 굴린다. 격자 밖이면 방향을 반대로 바꿔 굴린다.
	    2) 점수 : 도착 칸과 같은 숫자로 상하좌우 연결된 칸 수 x 그 숫자를 얻는다.
	    3) 회전 : 아랫면 > 칸 숫자면 시계 방향 90도, 작으면 반시계 90도, 같으면 그대로.

	  M번 동안 얻은 점수의 합을 출력한다.

	■ 풀이 방침
	  - 점수판을 미리 만든다 : 격자는 바뀌지 않으므로 칸마다 "같은 숫자 덩어리 크기 x 숫자"를
	    처음에 BFS로 한 번만 계산해 scoreBoard에 적어 둔다.
	    BFS가 끝났을 때 큐에 들어간 원소 수(wp)가 곧 덩어리 크기다.
	  - 주사위는 6면을 전개도 모양 그대로 이름 붙여 들고 굴릴 때마다 면을 옮긴다.

	          up                (북쪽 면)
	    left  top  right        (서쪽, 윗면, 동쪽)
	          down              (남쪽 면)
	          bottom            (아랫면)

	  - 방향 전환은 표 3개(반대, 시계, 반시계)로 처리한다.

	■ 주의할 점
	  [버그] simulate()에서 서쪽으로 굴릴 때 moveWest()가 아니라 moveEast()를 부른다.
	         -> else if (dir == WEST) moveWest();
	         고친 코드와 랜덤 입력 200개를 비교하면 155개에서 답이 다르다.
	  [주석 오류] dr/dc 위 주석이 "-, 동, 서, 남, 북"인데 실제 값은 3번이 북(-1), 4번이 남(+1)이다.
	         #define(NORTH 3, SOUTH 4)과 값은 맞고 주석만 틀렸다.
*/
#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

using namespace std;

#define MAX (20 +5)

#define EAST (1)
#define WEST (2)
#define NORTH (3)
#define SOUTH (4)

int T;
int N, M;
int MAP[MAX][MAX];

bool visit[MAX][MAX];
int scoreBoard[MAX][MAX]; // 그 칸에 도착했을 때 얻는 점수

// 전개도 모양으로 6면을 둔다.
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
// [주석 오류] 실제 순서는 -, 동, 서, 북, 남 이다. (#define NORTH 3, SOUTH 4와 일치)
int dr[] = { 0,0,0,-1,1 };
int dc[] = { 0,1,-1,0,0 };

// [수정] scanf 대신 인자로 받는다. M은 격자 크기가 아니라 "이동 횟수"다.
void input(int m, const vector<vector<int>>& board)
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

// 굴리기 4개 : tmp에 굴리기 전 값을 받아 두고 바뀌는 4면만 옮긴다.
// tmp 인덱스 : 0 up, 1 left, 2 top, 3 right, 4 down, 5 bottom

// 동쪽으로 굴림 : 윗면 -> 동, 동 -> 아래, 아래 -> 서, 서 -> 윗면 (up, down은 그대로)
void moveEast()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };

	cube.top = tmp[1];
	cube.right = tmp[2];
	cube.bottom = tmp[3];
	cube.left = tmp[5];
}

// 서쪽으로 굴림 : 윗면 -> 서, 서 -> 아래, 아래 -> 동, 동 -> 윗면
void moveWest()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };

	cube.top = tmp[3];
	cube.right = tmp[5];
	cube.bottom = tmp[1];
	cube.left = tmp[2];
}

// 북쪽으로 굴림 : 윗면 -> 북, 북 -> 아래, 아래 -> 남, 남 -> 윗면 (left, right는 그대로)
void moveNorth()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };

	cube.up = tmp[2];
	cube.top = tmp[4];
	cube.down = tmp[5];
	cube.bottom = tmp[0];
}

// 남쪽으로 굴림 : 윗면 -> 남, 남 -> 아래, 아래 -> 북, 북 -> 윗면
void moveSouth()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };

	cube.up = tmp[5];
	cube.top = tmp[0];
	cube.down = tmp[2];
	cube.bottom = tmp[4];
}

// (r, c)와 같은 숫자로 연결된 덩어리를 찾아 점수(숫자 x 크기)를 덩어리 전체에 적는다.
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

	// 큐에 들어간 칸들이 곧 덩어리이고, wp가 그 크기다.
	for (int i = 0; i < wp; i++)
	{
		int r, c;

		r = queue[i].r;
		c = queue[i].c;

		scoreBoard[r][c] = number * wp;
	}
}

// 모든 덩어리의 점수를 미리 계산한다. (격자는 변하지 않으므로 한 번이면 된다)
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

	// 처음 상태 : 윗면 1, 남 2, 동 3 -> 나머지는 7에서 뺀 값
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

		// 격자 밖이면 반대 방향으로 굴린다. (바뀐 방향은 계속 유지)
		if (nr<1 || nc<1 || nr>N || nc> N)
		{
			dir = changeDir[dir];
			nr = sr + dr[dir];
			nc = sc + dc[dir];
		}

		if (dir == EAST) moveEast();
		// [버그] moveWest()를 불러야 한다.
		else if (dir == WEST) moveWest();   // [버그수정] 원본은 moveEast()를 불렀다
		else if (dir == NORTH) moveNorth();
		else if (dir == SOUTH) moveSouth();

		score += scoreBoard[nr][nc];

		// 아랫면과 칸 숫자를 비교해 다음 방향을 정한다.
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
int solution(int m, vector<vector<int>> board)
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

	vector<vector<int>> board(n, vector<int>(n));
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
