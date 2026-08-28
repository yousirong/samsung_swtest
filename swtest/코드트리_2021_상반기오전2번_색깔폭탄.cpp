/*
	[코드트리] 2021 상반기 오전 2번 - 색깔 폭탄
	(백준 21609 "상어 중학교"와 형태가 같은 문제다. 다만 그룹 선택의 동점 처리 규칙이 다르니
	 아래 isPriority 설명을 그대로 따르는 것이 안전하다)

	■ 문제 요약
	  N x N 격자에 폭탄이 놓여 있다.

	     1 이상 : 색깔 폭탄 (숫자가 곧 색)
	     0      : 빨간(무지개) 폭탄. 어떤 색과도 함께 묶일 수 있다
	    -1      : 검은 폭탄. 절대 묶이지 않고 움직이지도 않는다
	    -2      : 빈칸

	  더 이상 터뜨릴 그룹이 없을 때까지 아래를 반복한다.

	    1) 가장 좋은 폭탄 그룹을 하나 고른다 (크기 2 이상이어야 한다)
	    2) 그 그룹을 터뜨리고 (그룹 크기)^2 만큼 점수를 얻는다
	    3) 중력 : 폭탄이 아래로 떨어진다
	    4) 격자 전체를 반시계 방향 90도 회전
	    5) 중력 : 다시 아래로 떨어진다

	  얻은 점수의 합을 출력한다.

	■ 그룹의 정의
	  "기준 폭탄"은 반드시 색깔 폭탄(1 이상)이어야 하고,
	  그 기준 색과 같은 색 또는 빨간 폭탄끼리 상하좌우로 이어진 덩어리가 하나의 그룹이다.
	  빨간 폭탄은 여러 그룹에 중복해서 속할 수 있으므로,
	  방문 배열을 그룹마다 새로 비워야 한다. (BFS 안에 지역 변수로 둔 이유가 이것이다)

	■ 그룹 고르기 : BFS를 두 번 쓴다
	    BFS()        : 후보 그룹의 크기와 빨간 폭탄 수, 기준 좌표만 조사한다 (격자는 건드리지 않는다)
	    deleteBomb() : 실제로 터뜨릴 그룹만 다시 훑으며 EMPTY로 지운다

	  "조사"와 "지우기"를 나눠 둔 덕분에, 모든 기준 칸을 안전하게 시험해 보고
	  가장 좋은 하나만 고른 뒤 그때 지울 수 있다.

	■ 우선순위 (isPriority)
	  이 코드가 사용하는 순서는 다음과 같다.

	    1) 그룹의 크기가 큰 것
	    2) 빨간 폭탄 수가 "적은" 것
	    3) 기준 좌표의 행이 "큰" 것
	    4) 기준 좌표의 열이 "작은" 것

	  기준 좌표는 BFS를 시작한 칸이고, 격자를 위->아래 / 왼쪽->오른쪽으로 훑으므로
	  같은 그룹을 여러 번 조사해도 기준 좌표는 그중 가장 위/왼쪽 칸으로 고정된다.

	  ※ 백준 21609는 2번이 "무지개 블록이 많은 것", 4번이 "열이 큰 것"으로 반대다.
	    두 문제의 조건이 다르니 옮겨 쓸 때 이 부분을 반드시 확인해야 한다.

	■ 중력과 회전
	    bombDownColumn() : 한 열을 아래에서 위로 훑으며, 밑이 빈칸이면 계속 내려보낸다.
	                       아래쪽부터 처리해야 먼저 자리를 잡은 폭탄 위로 쌓인다.
	                       검은 폭탄과 빈칸은 건드리지 않는다.
	    rotate()         : 반시계 90도. MAP[r][c] = 원본[c][N - r + 1]

	  회전 앞뒤로 중력을 한 번씩 적용해야 한다는 점이 순서상 핵심이다.

	■ 종료 조건
	  크기 2 이상인 그룹을 하나도 못 찾으면 selectedBomb.total이 초기값 -1로 남는다.
	  그 값을 그대로 종료 신호로 쓴다.
*/

#include <stdio.h>

#define MAX (20 + 5)
#define INF (0x7fff0000)

#define RED (0)      // 빨간(무지개) 폭탄
#define BLACK (-1)   // 검은 폭탄 (격자 바깥도 이 값으로 채운다)
#define EMPTY (-2)   // 빈칸

int T;
int N, M;
int MAP[MAX][MAX];

// ↑, →, ↓, ←
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0, -1 };

struct RC
{
	int r;
	int c;
};

RC queue[MAX * MAX];

// 조사한 그룹의 요약 정보
struct BOMB
{
	int total; // 그룹에 속한 폭탄 개수
	int red;   // 그중 빨간 폭탄 개수
	int r;     // 기준 좌표 (BFS를 시작한 칸)
	int c;
};

void input()
{
	scanf("%d %d\n", &N, &M);

	// 격자 바깥을 검은 폭탄으로 채워 두면 BFS와 중력에서 범위 검사가 필요 없다.
	// (검은 폭탄은 어떤 그룹에도 들어가지 않고 움직이지도 않으므로 벽 역할에 딱 맞는다)
	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= N + 1; c++)
			MAP[r][c] = BLACK;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);
}

void printMap(int map[MAX][MAX]) // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			copy[r][c] = original[r][c];
}

/*
	격자 전체를 반시계 방향 90도 회전

	  결과[r][c] = 원본[c][N - r + 1]

	제자리로 돌리면 아직 안 옮긴 값을 덮어쓰므로 스냅샷을 떠 두고 그쪽에서 읽는다.
*/
void rotate()
{
	int tmpMAP[MAX][MAX] = { 0 };

	copyMap(tmpMAP, MAP);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] = tmpMAP[c][N - r + 1];
}

/*
	(r, c)를 기준 폭탄으로 하는 그룹을 조사한다 (격자는 바꾸지 않는다)

	  같은 색이거나 빨간 폭탄인 칸끼리 상하좌우로 이어진 덩어리를 센다.
	  visit을 지역 변수로 둔 이유는, 빨간 폭탄이 여러 그룹에 겹쳐 속할 수 있어
	  그룹마다 방문 기록을 새로 시작해야 하기 때문이다.
*/
BOMB BFS(int r, int c)
{
	int rp, wp, bomb;
	bool visit[MAX][MAX] = { 0 };
	BOMB selectedBomb = { 0 };

	selectedBomb.total = 1;   // 기준 칸부터 센다
	selectedBomb.r = r;       // 기준 좌표는 BFS를 시작한 칸
	selectedBomb.c = c;

	rp = wp = 0;

	bomb = MAP[r][c];   // 이 그룹의 색

	queue[wp].r = r;
	queue[wp++].c = c;

	visit[r][c] = true;

	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 검은 폭탄(격자 바깥 포함)과 이미 센 칸은 건너뛴다
			if (MAP[nr][nc] == BLACK || visit[nr][nc] == true) continue;

			// 같은 색이거나 빨간 폭탄이면 같은 그룹이다.
			// 빈칸(-2)과 다른 색은 여기서 자연히 걸러진다.
			if (MAP[nr][nc] == bomb || MAP[nr][nc] == RED)
			{
				queue[wp].r = nr;
				queue[wp++].c = nc;

				visit[nr][nc] = true;

				selectedBomb.total++;

				if (MAP[nr][nc] == RED) selectedBomb.red++;
			}
		}
	}

	return selectedBomb;
}

/*
	(r, c)를 기준으로 하는 그룹을 실제로 터뜨린다 (EMPTY로 지운다)

	BFS와 같은 탐색이지만 이쪽은 격자를 직접 바꾼다.
	기준 칸의 색을 먼저 읽어 둔 뒤 지워야 한다는 점에 주의
	(먼저 EMPTY로 만들면 비교 기준이 사라진다).
*/
void deleteBomb(int r, int c) // BFS
{
	int rp, wp, bomb;
	bool visit[MAX][MAX] = { 0 };

	rp = wp = 0;

	bomb = MAP[r][c];    // 색을 먼저 읽고
	MAP[r][c] = EMPTY;   // 그다음에 지운다

	queue[wp].r = r;
	queue[wp++].c = c;

	visit[r][c] = true;

	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			if (MAP[nr][nc] == BLACK || visit[nr][nc] == true) continue;

			if (MAP[nr][nc] == bomb || MAP[nr][nc] == RED)
			{
				queue[wp].r = nr;
				queue[wp++].c = nc;

				visit[nr][nc] = true;
				MAP[nr][nc] = EMPTY;
			}
		}
	}
}

/*
	a가 b보다 우선순위가 높으면 true

	  1) 크기가 큰 것
	  2) 빨간 폭탄이 적은 것
	  3) 기준 행이 큰 것
	  4) 기준 열이 작은 것

	"값이 다르면 그 기준으로 결정, 같으면 다음 기준으로" 형태라 조건이 늘어나도 그대로 확장된다.
	각 줄의 부등호 방향이 규칙 그대로이니 옮겨 쓸 때 방향을 헷갈리지 않도록 주의.
*/
bool isPriority(BOMB a, BOMB b)
{
	if (a.total != b.total) return a.total > b.total;
	if (a.red != b.red) return a.red < b.red;
	if (a.r != b.r) return a.r > b.r;

	return a.c < b.c;
}

/*
	한 열에 중력을 적용한다

	아래쪽 행부터 훑어야 한다. 먼저 바닥에 닿은 폭탄 위로 다음 폭탄이 쌓이기 때문이다.
	검은 폭탄은 움직이지 않으므로 건너뛰고, 그 아래로도 내려가지 않는다
	(아래 칸이 EMPTY가 아니면 멈추기 때문).
*/
void bombDownColumn(int col)
{
	for (int r = N; r >= 1; r--)
	{
		if (MAP[r][col] == BLACK || MAP[r][col] == EMPTY) continue;

		int sr = r;
		while (1)
		{
			// 아래가 빈칸이 아니면 더 못 내려간다 (다른 폭탄, 검은 폭탄, 격자 바깥 모두)
			if (MAP[sr + 1][col] != EMPTY) break;

			if (MAP[sr + 1][col] == EMPTY)
			{
				int tmp = MAP[sr][col];
				MAP[sr][col] = MAP[sr + 1][col];
				MAP[sr + 1][col] = tmp;
			}

			sr++;
		}
	}
}

void bombDown()
{
	for (int col = 1; col <= N; col++)
		bombDownColumn(col);
}

int simulate()
{
	int score = 0;
	while (1)
	{
		BOMB selectedBomb = { 0 };

		// 어떤 후보든 처음 비교에서 이기도록 초기값을 최악으로 둔다.
		// (total = -1 이라 크기 비교에서 무조건 진다)
		selectedBomb.red = selectedBomb.c = INF;
		selectedBomb.total = selectedBomb.r = -1;

		// 모든 색깔 폭탄을 기준으로 그룹을 조사해 본다.
		// 위 -> 아래, 왼쪽 -> 오른쪽 순서라 같은 그룹의 기준 좌표는 항상 그중 가장 위/왼쪽 칸이 된다.
		for (int r = 1; r <= N; r++)
		{
			for (int c = 1; c <= N; c++)
			{
				// 기준 폭탄은 반드시 색깔 폭탄이어야 한다
				if (MAP[r][c] == BLACK || MAP[r][c] == RED || MAP[r][c] == EMPTY) continue;

				BOMB tmp = BFS(r, c);

				// 혼자짜리는 터뜨릴 수 없다
				if (tmp.total == 1) continue;

				if (isPriority(tmp, selectedBomb) == true)
					selectedBomb = tmp;
			}
		}

		// 터뜨릴 그룹이 하나도 없으면 종료
		if (selectedBomb.total == -1) break;

		score += (selectedBomb.total * selectedBomb.total);

		deleteBomb(selectedBomb.r, selectedBomb.c);

		// 중력 -> 반시계 회전 -> 중력. 이 순서를 지켜야 한다.
		bombDown();
		rotate();
		bombDown();
	}

	return score;
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
