/*
	[코드트리] 2024 하반기 오후 1번 - 메두사와 전사들

	[프로그래머스 제출용]  원본 : swtest/코드트리_2024_하반기오후1번_메두사와전사들.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).
	https://www.codetree.ai/training-field/frequent-problems/problems/medusa-and-warriors

	■ 문제 요약
	  N x N 마을은 도로(0)와 비도로(1)로 되어 있다. 메두사는 집에서 공원까지 도로만 따라
	  최단 경로로 이동한다(경로가 여러 개면 상 -> 하 -> 좌 -> 우 우선). 전사 M명이 메두사를 노린다.
	  집에서 공원으로 가는 길이 없으면 -1을 출력하고 끝난다.

	  매 턴은 아래 순서다.

	    1) 메두사 이동 : 경로를 따라 한 칸 간다. 그 칸에 전사가 있으면 그 전사들은 사라진다.
	    2) 메두사의 시선 : 상하좌우 중 한 방향을 바라본다. 시야는 그 방향으로 90도 부채꼴이다.
	       시야 안의 전사는 돌이 되어 이번 턴에 움직이지 못한다.
	       단, 다른 전사 뒤에 가려진 칸은 보이지 않는다.
	       돌이 되는 전사가 가장 많은 방향을 고르고, 같으면 상 -> 하 -> 좌 -> 우 순이다.
	    3) 전사 이동 (최대 두 번)
	       - 메두사와의 거리(|행 차이| + |열 차이|)가 줄어드는 방향으로만 한 칸 간다.
	       - 메두사의 시야 안 칸으로는 들어갈 수 없다.
	       - 첫 번째 이동은 상 -> 하 -> 좌 -> 우, 두 번째 이동은 좌 -> 우 -> 상 -> 하 우선이다.
	    4) 전사 공격 : 메두사와 같은 칸에 도착한 전사는 공격하고 사라진다.

	  메두사가 공원에 도착하기 전까지 턴마다
	  "전사들이 이동한 거리의 합, 돌이 된 전사 수, 공격한 전사 수"를 한 줄씩 출력하고,
	  공원에 도착하면 0을 출력한다.

	■ 풀이 방침
	  - 메두사의 경로는 처음에 BFS 한 번으로 통째로 구해 둔다(position[]).
	    이후 턴마다 position[p]로 한 칸씩 옮기기만 하면 된다.
	    BFS + before[] 역추적으로 경로를 얻고, 거꾸로 쌓였으므로 뒤집는다.
	    (도착점에서 거리를 재고 우선순위대로 한 칸씩 고르는 정석 방식과 무작위 1,778건을 비교해 경로가 모두 같았다)
	  - 시선은 scope[][] 한 장에 칠한다.
	        STRAIGHT(1) : 바라보는 방향 일직선
	        LEFT(2)     : 왼쪽 대각선 쪽 부채꼴
	        RIGHT(3)    : 오른쪽 대각선 쪽 부채꼴
	    값을 셋으로 나눠 둔 이유는 "그 전사가 어느 쪽 그림자를 만드는지" 알아야 하기 때문이다.

	■ 시야 부채꼴을 그리는 방법 (diagonal)
	  "대각선으로 k칸 간 점에서, 바라보는 방향으로 끝까지 직진"을 k = 1, 2, 3, ... 로 반복한다.
	  예를 들어 위를 볼 때 왼쪽 부채꼴은 ↖ 로 k칸 간 뒤 ↑ 로 쭉 칠한다.
	  대각선 방향은 (바라보는 방향 + 옆 방향)으로 만든다.
	      leftLook  : 상 -> 좌, 하 -> 우, 좌 -> 하, 우 -> 상
	      rightLook : 상 -> 우, 하 -> 좌, 좌 -> 상, 우 -> 하

	■ 가려지는 칸(그림자) 지우기
	  시야 안의 전사마다, 그 전사 뒤에 생기는 그림자를 0으로 지운다.
	      일직선 위의 전사     : 그 전사부터 바라보는 방향으로 일직선
	      왼쪽 부채꼴 위의 전사 : 일직선 + 왼쪽 부채꼴
	      오른쪽 부채꼴 위의 전사 : 일직선 + 오른쪽 부채꼴
	  전사 자기 칸은 지우지 않는다(그 전사는 보인다).
	  가려진 전사의 그림자는 앞 전사의 그림자 안에 다 들어가므로 전사를 어떤 순서로 처리해도 결과가 같다.
	  벽(비도로)은 시야를 가리지 않는다. 시야를 가리는 것은 전사뿐이다.

	■ 전사 이동
	  돌이 된 전사(scope != 0 인 칸에 있는 전사)는 움직이지 않는다.
	  메두사 쪽으로만 가므로 격자를 벗어날 일이 없고, 시야 칸만 피하면 된다.
	  같은 함수를 first 플래그만 바꿔 두 번 부른다 (첫 이동 / 두 번째 이동의 우선순위가 다르다).

	■ 주의할 점
	  - visit은 bool인데 visit[out] + 1 을 넣는다. bool이라 그냥 true가 되며, 방문 여부로만 쓴다.
	  - 전역 이름 end 는 std::end 와 겹친다. 지금은 stdio.h만 써서 괜찮지만
	    프로그래머스 제출용으로 옮길 때는 이름을 바꿔야 한다.
*/
#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

using namespace std;

#define MAX_N (50+5)
#define MAX_M (300+30)

#define WALL (1)

#define UP (0)
#define DOWN (1)
#define LEFT (2)
#define RIGHT (3)

#define STRAIGHT (1)

int T;

int N, M;
int MAP[MAX_N][MAX_N];
bool visit[MAX_N][MAX_N]; // BFS 방문 체크
int scope[MAX_N][MAX_N]; // 메두사의 시선 처리

struct RC
{
	int r;
	int c;
};

// [이름변경] end -> endPos (using namespace std; 를 쓰면 std::end 와 겹친다)
RC start, endPos, medusa;
RC queue[MAX_N * MAX_N];
RC before[MAX_N][MAX_N];
RC position[MAX_N * MAX_N]; // 메두사의 경로
int pcnt; // 메두사의 경로 길이

struct RCD
{
	int r;
	int c;
	bool dead; // 사망 처리
};

RCD warrior[MAX_M];

struct ANSWER
{
	int distance; // 모든 전사가 이동한 거리의 합
	int stone; // 메두사로 인해 돌이 된 전사의 수
	int attack; // 메두사로 공격한 전사의 수
};

ANSWER answer;

int turnResult[MAX_N * MAX_N][3];   // [추가] 턴마다의 결과 (원본은 바로 printf 했다)
int turnCount;
bool noPath;                        // [추가] 공원으로 가는 길이 없는가

// ↑, ↓, ←, → (상, 하, 좌, 우)
int dr[] = { -1,1,0,0 };
int dc[] = { 0,0,-1,1 };

// [수정] scanf 대신 인자로 받는다
// board = N x N (0 도로, 1 비도로), home/park = {행, 열}, warriors[m] = {행, 열}  (좌표는 0부터)
void input(const vector<vector<int>>& board, const vector<int>& home, const vector<int>& park,
	const vector<vector<int>>& warriors)
{
	N = (int)board.size();        // [수정] scanf("%d %d %d %d %d %d", ...) 대체
	M = (int)warriors.size();

	start.r = home[0] + 1;
	start.c = home[1] + 1;
	endPos.r = park[0] + 1;
	endPos.c = park[1] + 1;

	pcnt = 0;

	for (int m = 0; m < M; m++)
	{
		warrior[m].r = warriors[m][0] + 1;   // [수정] scanf 대체
		warrior[m].c = warriors[m][1] + 1;
		warrior[m].dead = false;
	}

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] = board[r - 1][c - 1];   // [수정] scanf 대체

	// [추가] 재호출 대비 : 원본은 visit(BFS 방문 표시)을 초기화하지 않았다
	for (int r = 0; r < MAX_N; r++)
		for (int c = 0; c < MAX_N; c++)
			visit[r][c] = false;

	turnCount = 0;
	noPath = false;
}

void printMap(int map[MAX_N][MAX_N]) // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void printStatus() // for debug
{
	int tmpMAP[MAX_N][MAX_N] = { 0 };

	tmpMAP[medusa.r][medusa.c] = -1;

	for (int m = 0; m < M; m++)
	{
		if (warrior[m].dead == true) continue;
		
		int r, c;

		r = warrior[m].r;
		c = warrior[m].c;

		tmpMAP[r][c] = m;
	}

	printMap(tmpMAP);
}

void BFS()
{
	int rp, wp;
	int sr, sc, er, ec;

	rp = wp = 0;

	sr = start.r;
	sc = start.c;
	er = endPos.r;
	ec = endPos.c;

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

			position[pcnt].r = er;
			position[pcnt++].c = ec;

			while (1)
			{
				int br, bc; //이전좌표

				br = before[tr][tc].r;
				bc = before[tr][tc].c;

				if (br == sr && bc == sc) break;

				position[pcnt].r = br;
				position[pcnt++].c = bc;

				tr = br;
				tc = bc;
			}

			for (int i = 0; i < (pcnt / 2); i++)
				// 도착점부터 거꾸로 쌓았으므로 뒤집어 출발 다음 칸부터 순서대로 만든다.
			{
				RC tmp = position[i];
				position[i] = position[pcnt - 1 - i];
				position[pcnt - 1 - i] = tmp;
			}
		}

		for (int i = 0; i < 4; i++)
		{
			int nr = out.r + dr[i];
			int nc = out.c + dc[i];

			if (nr<1 || nc<1 || nr>N || nc>N) continue;
			if (visit[nr][nc] != 0 || MAP[nr][nc] == WALL) continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = visit[out.r][out.c] + 1;

			before[nr][nc] = out;
		}
	}
}

void moveMedusa(int index)
{
	medusa = position[index];

	for (int m = 0; m < M; m++)
	{
		if (warrior[m].dead == true) continue;

		int wr, wc;

		wr = warrior[m].r;
		wc = warrior[m].c;

		if (wr == medusa.r && wc == medusa.c)
			// 메두사가 들어온 칸의 전사는 사라진다.
			warrior[m].dead = true;
	}
}

void straight(int r, int c, int dir, int value)
{
	int sr, sc;

	sr = r;
	sc = c;

	while (1)
	{
		int nr, nc;

		nr = sr + dr[dir];
		nc = sc + dc[dir];

		if (nr <1 || nc<1 || nr>N || nc>N) break;

		scope[nr][nc] = value;

		sr = nr;
		sc = nc;
	}
}

void diagonal(int r, int c, int dir, int nDir, int value)
{
	int step = 1;
	while (1)
	{
		int sr, sc;

		sr = r + (dr[dir] + dr[nDir]) * step;
		// 대각선으로 step칸 간 점 -> 거기서 바라보는 방향으로 끝까지 칠한다.
		sc = c + (dc[dir] + dc[nDir]) * step;

		if (sr<1 || sc<1 || sr>N || sc>N) break;

		scope[sr][sc] = value;

		while (1)
		{
			int nr, nc;

			nr = sr + dr[dir];
			nc = sc + dc[dir];

			if (nr<1 || nc<1 || nr>N || nc>N) break;

			scope[nr][nc] = value;

			sr = nr;
			sc = nc;
		}
		step++;
	}
}

// ↑, ↓, ←, → (상, 하, 좌, 우 / 0, 1, 2, 3)
void leftLook(int r, int c, int dir, int value)
{
	int changeDir[4] = { 2,3,1,0 };
	int leftDir = changeDir[dir];

	diagonal(r, c, dir, leftDir, value);
}

void rightLook(int r, int c, int dir, int value)
{
	int changeDir[4] = { 3,2,0,1 };
	int rightDir = changeDir[dir];

	diagonal(r, c, dir, rightDir, value);
}

int countStoneWarrior(int dir)
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scope[r][c] = 0;

	int mr, mc;

	mr = medusa.r;
	mc = medusa.c;

	straight(mr, mc, dir, STRAIGHT);
	leftLook(mr, mc, dir, LEFT);
	rightLook(mr, mc, dir, RIGHT);

	for (int m = 0; m < M; m++)
	{
		if (warrior[m].dead == true) continue;

		int wr = warrior[m].r;
		int wc = warrior[m].c;

		if (scope[wr][wc] == STRAIGHT) straight(wr, wc, dir, 0);
		// 시야 안의 전사 뒤쪽(그림자)을 0으로 지운다. 전사 자기 칸은 그대로 둔다.
		else if (scope[wr][wc] == LEFT)
		{
			straight(wr, wc, dir, 0);
			leftLook(wr, wc, dir, 0);
		}
		else if (scope[wr][wc] == RIGHT)
		{
			straight(wr, wc, dir, 0);
			rightLook(wr, wc, dir, 0);
		}
	}

	int count = 0;

	for (int m = 0; m < M; m++)
	{
		if (warrior[m].dead == true) continue;

		int wr, wc;

		wr = warrior[m].r;
		wc = warrior[m].c;

		if (scope[wr][wc] != 0) count++;
	}

	return count;
}

void lookAt()
{
	int maxCount, maxDir;

	maxCount = maxDir = 0;

	for (int i = 0; i < 4; i++)
	{
		int tmp = countStoneWarrior(i);
		if (maxCount < tmp)
			// "더 클 때만" 갱신 -> 동점이면 상 하 좌 우 순으로 앞선 방향이 남는다.
		{
			maxCount = tmp;
			maxDir = i;
		}
	}

	countStoneWarrior(maxDir);
	// 고른 방향으로 scope를 다시 그려 둔다 (전사 이동 때 이 시야를 쓴다).

	answer.stone = maxCount;
}

int checkRow(RCD w)
{
	int mr = medusa.r;
	int wr = w.r;

	// mr > wr -> 메두사는 아래에
	// mr = wr -> 같은 행, 상하로 움직일 수 없음.
	// mr < wr -> 메두사는 위에
	return mr - wr;
}

int checkCol(RCD w)
{
	int mc = medusa.c;
	int wc = w.c;

	// mc > wc -> 메두사는 오른쪽에
	// mc = wc -> 같은 열, 좌우로 움직일 수 없음.
	// mc < wc -> 메두사는 왼쪽에
	return mc - wc;
}

bool checkScope(RCD w, int dir)
{
	int nr, nc;

	nr = w.r + dr[dir];
	nc = w.c + dc[dir];

	if (scope[nr][nc] != 0) return false;
	// 메두사 시야 안으로는 들어갈 수 없다.

	return true;
}

void moveWarrior(bool first)
{
	for (int m = 0; m < M; m++)
	{
		RCD w = warrior[m];

		if (w.dead == true) continue;
		if (scope[w.r][w.c] != 0) continue;
		// 돌이 된 전사는 이번 턴에 움직이지 않는다.

		int upDown = checkRow(w);
		int leftRight = checkCol(w);

		int direction = -1;

		// 메두사를 향해서 이동하므로 격자의 바깥으로 나가지 않는다.
		if (first == true)
		{
			if (upDown < 0 && checkScope(w, UP) == true) direction = UP;
			else if (upDown > 0 && checkScope(w, DOWN) == true) direction = DOWN;
			else if (leftRight < 0 && checkScope(w, LEFT) == true) direction = LEFT;
			else if (leftRight > 0 && checkScope(w, RIGHT) == true) direction = RIGHT;
		}
		else
		{
			if (leftRight < 0 && checkScope(w, LEFT) == true) direction = LEFT;
			else if (leftRight > 0 && checkScope(w, RIGHT) == true) direction = RIGHT;
			else if (upDown < 0 && checkScope(w, UP) == true) direction = UP;
			else if (upDown > 0 && checkScope(w, DOWN) == true) direction = DOWN;
		}

		if (direction == -1) continue;

		int nr, nc;

		nr = w.r + dr[direction];
		nc = w.c + dc[direction];

		warrior[m].r = nr;
		warrior[m].c = nc;

		answer.distance++;

		if (nr == medusa.r && nc == medusa.c)
		{
			answer.attack++;
			warrior[m].dead = true;
		}
	}
}

void simulate()
{
	if (pcnt == 0)
	{
		noPath = true;   // [수정] printf("-1") -> 표시만 해 두고 solution()이 [[-1]]을 돌려준다
		return;
	}

	for (int p = 0; p < pcnt - 1; p++)
		// 마지막 칸(공원)에 도착하는 턴은 0만 출력하므로 pcnt - 1 까지만 돈다.
	{
		answer.distance = answer.stone = answer.attack = 0;

		moveMedusa(p);

		lookAt();

		moveWarrior(true);
		moveWarrior(false);

		turnResult[turnCount][0] = answer.distance;   // [수정] printf -> 배열에 담기
		turnResult[turnCount][1] = answer.stone;
		turnResult[turnCount][2] = answer.attack;
		turnCount++;
	}

	// [수정] printf("0") -> 공원 도착 줄은 solution()에서 붙인다
}

// [수정] main() -> solution().
// 원본은 턴마다 "이동 거리 합, 돌이 된 수, 공격한 수"를 한 줄씩 출력하고 마지막에 0을 출력했다.
// 같은 내용을 줄 단위로 담아 반환한다. 공원으로 가는 길이 없으면 [[-1]] 이다.
vector<vector<int>> solution(vector<vector<int>> board, vector<int> home, vector<int> park, vector<vector<int>> warriors)
{
	input(board, home, park, warriors);

	BFS();

	simulate();

	vector<vector<int>> answer;
	if (noPath == true)
	{
		answer.assign(1, vector<int>(1, -1));
		return answer;
	}

	answer.assign(turnCount + 1, vector<int>());
	for (int t = 0; t < turnCount; t++)
		answer[t] = vector<int>(turnResult[t], turnResult[t] + 3);
	answer[turnCount] = vector<int>(1, 0);   // 공원에 도착한 턴

	return answer;
}
