/*
	[코드트리] 2024 상반기 오후 1번 - 마법의 숲 탐색

	[프로그래머스 제출용]  원본 : swtest/코드트리_2024_상반기오후1번_마법의숲탐색.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).
	https://www.codetree.ai/training-field/frequent-problems/problems/magical-forest-exploration

	■ 문제 요약
	  R x C 숲에 골렘 K개가 차례로 들어온다. 골렘은 십자 모양(중앙 + 상하좌우)이고
	  출구가 네 방향 중 한 곳에 있다(0 북, 1 동, 2 남, 3 서).

	  골렘 하나는 이렇게 움직인다 (숲 위쪽 바깥에서 시작한다).

	    1) 남쪽으로 한 칸 갈 수 있으면 남쪽으로 간다.
	    2) 못 가면 서쪽으로 회전하며 내려간다 (서쪽으로 한 칸 + 남쪽으로 한 칸, 출구는 반시계로 한 칸 회전).
	    3) 그것도 못 가면 동쪽으로 회전하며 내려간다 (출구는 시계로 한 칸 회전).
	    4) 셋 다 못 하면 멈춘다.

	  멈춘 자리가 숲을 벗어나 있으면(중앙이 3행보다 위) 숲을 전부 비우고 다음 골렘을 받는다.
	  숲에 자리를 잡았으면 정령이 골렘 안에서 움직인다.
	    - 같은 골렘 안에서는 어디로든 갈 수 있다.
	    - 다른 골렘으로는 "지금 골렘의 출구"를 통해서만 넘어갈 수 있다.
	  정령이 도달할 수 있는 가장 남쪽 행 번호를 누적해 더한 값이 답이다.

	■ 좌표를 위로 2칸 늘려 쓰는 이유
	  골렘은 숲 바깥(위쪽)에서 시작해 내려온다. 그래서 실제 1행을 배열의 3행으로 두고
	  위쪽 두 줄(1, 2행)을 골렘이 대기하는 공간으로 쓴다.
	    - 시작은 언제나 g.r = 1 (중앙이 배열 1행 = 숲 밖)
	    - 멈춘 뒤 g.r <= 3 이면 십자의 위쪽 팔이 숲 밖이라 실패 -> 숲을 비운다
	    - 답을 낼 때는 배열 행에서 2를 빼 실제 행 번호로 바꾼다 (BFS의 maxR - 2)

	■ 이동 가능 판정
	  checkSouth  : 남쪽으로 한 칸 갈 때 새로 밟는 세 칸(↙, ↓↓, ↘)이 비어 있는지 본다.
	  checkWest   : 서쪽 세 칸이 비어 있고, 그 자리에서 남쪽으로도 갈 수 있어야 한다(회전+하강이라서).
	  checkEast   : 동쪽도 같은 방식이다.
	  그래서 checkWest / checkEast 는 마지막에 checkSouth 를 한 번 더 부른다.

	■ 한 칸에 두 값을 담는 방법 (골렘 번호 + 종류)
	  MAP에 "골렘 번호 x 10 + 종류"를 적는다. 종류는 몸통 1, 중앙 2, 출구 3이다.
	      골렘 번호 : MAP / 10 * 10
	      종류      : MAP % 10
	  덕분에 정령이 이동할 때 "같은 골렘인가"와 "여기가 출구인가"를 한 값에서 바로 읽을 수 있다.

	■ 정령의 이동 (BFS)
	  같은 골렘 안이면 자유롭게 퍼지고, 다른 골렘으로는 지금 칸이 출구일 때만 넘어간다.
	      if (type != EXIT && golemID != ngolemID) continue;
	  퍼진 칸 중 가장 큰 행이 그 골렘에서 정령이 도달한 최남단이다.

	■ 주의할 점
	  - 숲을 비우는 경우(골렘이 자리 못 잡음)에는 그때까지 쌓인 답은 그대로 두고 격자만 지운다.
	  - checkSouth의 gr == R + 1 검사는 "중앙이 이미 마지막 행"이라 더 내려갈 수 없다는 뜻이다.
	    (배열 기준으로 실제 마지막 행은 R + 2, 그 위 칸이 R + 1)
*/
#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

using namespace std;

#define MAX (70+10)
#define MAX_K (1000+10)	

#define BODY (1)
#define CENTER (2)
#define EXIT (3)
#define GOLEM_ID (10)

int T;

int R, C, K;

int MAP[MAX][MAX];
int start_c[MAX_K];
int exit_d[MAX_K];

struct RC
{
	int r;
	int c;
};

RC queue[MAX * MAX];

struct GOLEM
{
	int r;
	int c;
	int dir;
	int id;
};

// 북, 동, 남, 서
int dr[] = { -1,0,1,0 };
int dc[] = { 0,1,0,-1 };

// [수정] scanf 대신 인자로 받는다
// golems[k] = {출발 열, 출구 방향}  (k+1번째로 들어오는 골렘)
void input(int r, int c, const vector<vector<int>>& golems)
{
	R = r;                        // [수정] scanf("%d %d %d", &R, &C, &K) 대체
	C = c;
	K = (int)golems.size();

	for (int i = 0; i < MAX; i++)
		for (int j = 0; j < MAX; j++)
			MAP[i][j] = 0;

	for (int k = 1; k <= K; k++)
	{
		start_c[k] = golems[k - 1][0];   // [수정] scanf 대체
		exit_d[k] = golems[k - 1][1];
	}
}

void printMap() //for debug
{
	for (int r = 3; r <= R + 2; r++)
	{
		for (int c = 1; c <= C; c++)
			printf("%2d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// 남쪽
bool checkSouth(GOLEM golem)
{
	int gr, gc;

	gr = golem.r;
	gc = golem.c;

	if (gr == R + 1) return false;
	// 중앙이 더 내려가면 십자의 아래 팔이 숲을 벗어난다.

	int nr[3] = { 0 };
	int nc[3] = { 0 };

	nr[0] = gr + 1;
	nc[0] = gc - 1;

	nr[1] = gr + 2;
	nc[1] = gc;

	nr[2] = gr + 1;
	nc[2] = gc + 1;

	for (int i = 0; i < 3; i++)
		if (MAP[nr[i]][nc[i]] != 0) return false;

	return true;
}

// 서쪽 + 남쪽
bool checkWest(GOLEM golem)
{
	int gr, gc;

	gr = golem.r;
	gc = golem.c;

	if (gc == 2) return false;

	int nr[3] = { 0 };
	int nc[3] = { 0 };

	nr[0] = gr - 1;
	nc[0] = gc - 1;

	nr[1] = gr;
	nc[1] = gc-2;

	nr[2] = gr + 1;
	nc[2] = gc - 1;

	for (int i = 0; i < 3; i++)
		if (MAP[nr[i]][nc[i]] != 0) return false;

	GOLEM tmpGolem = { 0 };

	tmpGolem.r = gr;
	tmpGolem.c = gc-1;

	return checkSouth(tmpGolem);
	// 회전 이동은 "옆으로 한 칸 + 남쪽으로 한 칸"이라 남쪽도 확인해야 한다.
}

// 동쪾 + 남쪽
bool checkEast(GOLEM golem)
{
	int gr, gc;

	gr = golem.r;
	gc = golem.c;

	if (gc == C - 1) return false;

	int nr[3] = { 0 };
	int nc[3] = { 0 };

	nr[0] = gr -1;
	nc[0] = gc + 1;

	nr[1] = gr;
	nc[1] = gc + 2;

	nr[2] = gr + 1;
	nc[2] = gc + 1;

	for (int i = 0; i < 3; i++)
		if (MAP[nr[i]][nc[i]] != 0) return false;

	GOLEM tmpGolem = { 0 };

	tmpGolem.r = gr;
	tmpGolem.c = gc+1;

	return checkSouth(tmpGolem);
	// 회전 이동은 "옆으로 한 칸 + 남쪽으로 한 칸"이라 남쪽도 확인해야 한다.
}

void setGolem(GOLEM golem)
{
	int gr, gc, gid, dir;

	gr = golem.r;
	gc = golem.c;
	gid = golem.id;
	dir = golem.dir;

	MAP[gr][gc] = CENTER + gid;

	for (int i = 0; i < 4; i++)
	{
		int nr, nc;

		nr = gr + dr[i];
		nc = gc + dc[i];

		MAP[nr][nc] = BODY + gid;
	}

	MAP[gr + dr[dir]][gc + dc[dir]] = EXIT + gid;
	// 출구는 몸통 한 칸을 출구 표시로 덮어쓴다 (번호 x 10 + 3).
}

int BFS(GOLEM golem)
{
	int rp, wp;
	bool visit[MAX][MAX] = { 0 };

	rp = wp = 0;

	queue[wp].r = golem.r;
	queue[wp++].c = golem.c;

	visit[golem.r][golem.c] = true;

	int maxR = 0;
	while (rp < wp)
	{
		RC out = queue[rp++];

		if (maxR < out.r) maxR = out.r;

		int golemID = (MAP[out.r][out.c] / GOLEM_ID) * GOLEM_ID;
		// 한 값에서 골렘 번호와 종류를 같이 읽는다.
		int type = MAP[out.r][out.c] % GOLEM_ID;

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			if (nr<1 || nc<1 || nr>(R + 2) || nc>C) continue;
			if (MAP[nr][nc] == 0 || visit[nr][nc] == true) continue;

			int ngolemID = (MAP[nr][nc] / GOLEM_ID) * GOLEM_ID;

			if (type != EXIT && golemID != ngolemID) continue;
			// 다른 골렘으로 넘어가려면 지금 칸이 출구여야 한다.

			// 뽑은 좌표의 타입이 출구가 아니고
			// 다음 좌표의 ID가 다른 경우
			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = true;
		}
	}
	return maxR - 2;
	// 배열 행에서 2를 빼면 실제 숲의 행 번호가 된다.
}


int simulate(GOLEM golem)
{
	GOLEM g = golem;

	while (1)
	{
		if (checkSouth(g) == true) g.r = g.r + 1;
		else if (checkWest(g) == true) // 서쪽 + 남쪽
		{
			g.r = g.r + 1;
			g.c = g.c - 1;
			g.dir = (g.dir - 1 + 4) % 4;
		}
		else if (checkEast(g) == true) // 동쪽 + 남쪽
		{
			g.r = g.r + 1;
			g.c = g.c + 1;
			g.dir = (g.dir + 1) % 4;
		}
		else
			break;
	}

	if (g.r <= 3)
	// 십자의 위쪽 팔이 숲 밖이면 자리를 잡지 못한 것이다 -> 숲을 전부 비운다.
	{
		for (int r = 0; r < MAX; r++)
			for (int c = 0; c < MAX; c++)
				MAP[r][c] = 0;
		
		return 0;
	}

	setGolem(g);

	return BFS(g);
}

// [수정] main() -> solution(). 골렘을 차례로 넣으며 정령이 닿은 행을 더한다.
int solution(int r, int c, vector<vector<int>> golems)
{
	input(r, c, golems);

	int sum = 0;
	for (int k = 1; k <= K; k++)
	{
		GOLEM g = { 0 };

		g.r = 1;
		g.c = start_c[k];
		g.dir = exit_d[k];
		g.id = k * GOLEM_ID;

		int row = simulate(g);

		sum += row;
	}

	return sum;   // [수정] printf -> return
}
