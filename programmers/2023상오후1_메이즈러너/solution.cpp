/*
	[코드트리] 2023 상반기 오후 1번 - 메이즈 러너

	[프로그래머스 제출용]  원본 : swtest/코드트리_2023_상반기오후1번_메이즈러너.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).
	https://www.codetree.ai/training-field/frequent-problems/problems/maze-runner

	■ 문제 요약
	  N x N 미로에 참가자 M명과 출구 하나가 있다. 각 칸은 0(빈칸) 또는 1~9(벽의 내구도)다.
	  K초 동안 아래를 반복한다.

	    1) 모든 참가자가 동시에 한 칸 움직인다.
	       출구까지의 최단 거리(|행 차이| + |열 차이|)가 줄어드는 방향으로만 가고,
	       상하로 줄일 수 있으면 상하, 아니면 좌우로 간다. 벽이면 못 가고 제자리에 있는다.
	       움직인 참가자 수만큼 이동 거리가 쌓인다.
	    2) 출구에 도착한 참가자는 즉시 탈출한다(이후 움직이지도, 회전에 포함되지도 않는다).
	    3) 모두 탈출하지 못했으면 미로를 회전한다.
	       "탈출하지 못한 참가자 한 명 이상과 출구를 함께 포함하는 가장 작은 정사각형"을 고른다.
	       여러 개면 좌상단 행이 작은 것, 그다음 열이 작은 것.
	       그 정사각형을 시계 방향 90도로 돌리고, 회전한 구역의 벽은 내구도가 1 줄어든다.

	  K초가 지나거나 모두 탈출하면 끝난다.
	  출력은 두 줄이다. 첫 줄은 참가자들의 총 이동 거리, 둘째 줄은 마지막 출구 좌표다.

	■ 풀이 방침
	  - 이동은 "상하 먼저, 좌우 나중"이라는 우선순위만 지키면 된다.
	    출구 쪽으로만 가므로 격자를 벗어날 일이 없어 경계 검사가 필요 없다.
	        checkRow    : 출구가 위(음수) / 같은 행(0) / 아래(양수)
	        checkColumn : 출구가 왼쪽(음수) / 같은 열(0) / 오른쪽(양수)
	  - 회전할 정사각형은 크기 2부터 키워 가며 처음 찾은 것을 쓴다.
	    "가장 작은 것 -> 행이 작은 것 -> 열이 작은 것"이 곧 for문 순서(size, r, c)라 별도 비교가 없다.
	  - 참가자와 출구가 그 안에 있는지는 임시 격자에 사람과 출구를 찍어 두고 확인한다
	    (사람 = m + PLAYER_NUMBER, 출구 = DESTINATION).
	    벽 내구도(1~9)와 섞이지 않도록 사람 번호를 10부터 쓴 것이다.

	■ 좌표도 같이 회전시키는 방법
	  rotateRC는 "그 좌표에만 1을 찍은 빈 격자"를 만들어 같은 회전을 돌린 뒤 1을 다시 찾는다.
	  회전 공식을 좌표용으로 따로 쓰지 않아도 되고, 회전 구역 밖의 좌표는 그대로 남는다.
	  (map[sr+r][sc+c] = tmp[size-1-c][r] 이 시계 방향 90도다)

	■ 주의할 점
	  [확인 필요] getSqureInfo가 조건을 만족하는 정사각형을 못 찾으면 {0, 0, 0}을 돌려준다.
	              그 뒤 rotateRC가 (0, 0)을 반환해 출구와 참가자 좌표가 (0, 0)이 된다.
	              문제 조건상 남은 참가자와 출구를 포함하는 정사각형은 항상 있으므로 생기지 않는다.
	  - 내구도 감소는 회전한 구역 안의 "벽(0이 아닌 칸)"만 대상이다. 참가자와 출구는 MAP에 없다.
*/
#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

using namespace std;

#define MAX (10+5)
#define PLAYER_NUMBER (10)
#define DESTINATION (-1)

#define UP (0)
#define RIGHT (1)
#define DOWN (2)
#define LEFT (3)

int T;

int N, M, K;
int MAP[MAX][MAX];

struct RCE
{
	int r;
	int c;
	bool escape; // player 탈출 확인
};

RCE player[MAX];
int answerSum;   // [추가] simulate가 printf 하던 총 이동 거리를 담아 둔다
RCE destination;

struct RCS
{
	int r;
	int c;
	int size; // 회전을 위한 크기
};

// ↑, →, ↓, ←
int dr[] = { -1,0,1,0 };
int dc[] = { 0,1,0,-1 };

// [수정] scanf 대신 인자로 받는다
// board = 미로(0 빈칸, 1~9 벽 내구도), players[i] = {행, 열}, exitPos = {행, 열}
void input(int k, const vector<vector<int>>& board, const vector<vector<int>>& players,
	const vector<int>& exitPos)
{
	N = (int)board.size();        // [수정] scanf("%d %d %d", &N, &M, &K) 대체
	M = (int)players.size();
	K = k;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] = board[r - 1][c - 1];   // [수정] scanf 대체

	for (int m = 0; m < M; m++)
	{
		player[m].r = players[m][0];   // [수정] scanf 대체
		player[m].c = players[m][1];
		player[m].escape = false;
	}

	destination.r = exitPos[0];   // [수정] scanf 대체
	destination.c = exitPos[1];
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

void printStatus() // for debug
{
	int tmpMAP[MAX][MAX] = { 0 };
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			tmpMAP[r][c] = MAP[r][c];

	for (int m = 0; m < M; m++)
	{
		RCE p = player[m];

		printf("%d : %d, %d (exit : %d)\n", m, p.r, p.c, p.escape);
		if (p.escape == false)
			tmpMAP[p.r][p.c] = m + PLAYER_NUMBER;

		putchar('\n');

		tmpMAP[destination.r][destination.c] = DESTINATION;

		printMap(tmpMAP);
	}
}

void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			copy[r][c] = original[r][c];
}

bool check()
{
	for (int m = 0; m < M; m++)
		if (player[m].escape == false) return false;

	return true;
}

int checkRow(RCE player)
{
	int dr = destination.r;
	int pr = player.r;
	// dr > pr -> 출구는 아래에
	// dr = pr -> 같은 행, 상하로 움직일 수 없음.
	// dr < pr -> 출구는 위에
	return dr - pr;
}

int checkColumn(RCE player)
{
	int dc = destination.c;
	int pc = player.c;
	// dc > pc -> 출구는 오른쪽에
	// dc = pc -> 같은 열, 좌우로 움직일 수 없음.
	// dc < pc -> 출구는 왼쪽에
	return dc - pc;
}

int move()
{
	int step = 0;
	for (int m = 0; m < M; m++)
	{
		if (player[m].escape == true) continue;

		int nextR[4] = { 0 };
		int nextC[4] = { 0 };

		for (int i = 0; i < 4; i++)
		{
			nextR[i] = player[m].r + dr[i];
			nextC[i] = player[m].c + dc[i];
		}

		int upDown = checkRow(player[m]);
		int leftRight = checkColumn(player[m]);

		int direction = -1;

		// 항상 출구를 향해 움직이기 때문에 격자를 벗어나지 않음.
			// 상하로 줄일 수 있으면 상하, 아니면 좌우. (벽이면 그 방향은 못 쓴다)
		if (upDown < 0 && MAP[nextR[UP]][nextC[UP]] == 0) direction = UP;
		else if (upDown > 0 && MAP[nextR[DOWN]][nextC[DOWN]] == 0)direction = DOWN;
		else if (leftRight < 0 && MAP[nextR[LEFT]][nextC[LEFT]] == 0)direction = LEFT;
		else if (leftRight > 0 && MAP[nextR[RIGHT]][nextC[RIGHT]] == 0)direction = RIGHT;

		if (direction == -1) continue;
			// 네 방향 모두 막혔으면 제자리 (이동 거리도 늘지 않는다)

		player[m].r = player[m].r + dr[direction];
		player[m].c = player[m].c + dc[direction];

		if (player[m].r == destination.r && player[m].c == destination.c)
			player[m].escape = true;

		step++;
	}
	return step;
}


bool rotateCheck(int map[MAX][MAX], int sr, int sc, int size)
{
	bool destinationCheck, playerCheck;

	destinationCheck = playerCheck = false;

	for (int r = sr; r < sr + size; r++)
	{
		for (int c = sc; c < sc + size; c++)
		{
			if (map[r][c] >= PLAYER_NUMBER) playerCheck = true;
			else if (map[r][c] == DESTINATION) destinationCheck = true;
		}
	}
	return playerCheck && destinationCheck;
}

RCS getSqureInfo()
{
	RCS ret = { 0 };
	for (int size = 2; size <= N; size++)
		// 크기 2부터 키우고, 같은 크기면 행 -> 열 순서로 본다. 처음 찾은 것이 곧 정답이다.
	{
		for (int r = 1; r <= N - size + 1; r++)
		{
			for (int c = 1; c <= N - size + 1; c++)
			{
				int tmpMAP[MAX][MAX] = { 0 };

				copyMap(tmpMAP, MAP);

				for (int m = 0; m < M; m++)
				{
					if (player[m].escape == true) continue;

					tmpMAP[player[m].r][player[m].c] = m + PLAYER_NUMBER;
				}

				tmpMAP[destination.r][destination.c] = DESTINATION;
				if (rotateCheck(tmpMAP, r, c, size) == true)
				{
					ret.r = r;
					ret.c = c;
					ret.size = size;

					return ret;
				}
			}
		}
	}
	
	// for debug
	//ret.r = ret.c = ret.size = 1;
	return ret;
}


void rotate(int map[MAX][MAX], int sr, int sc, int size)
{
	int tmpMAP[MAX][MAX] = { 0 };

	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			tmpMAP[r][c] = map[sr + r][sc + c];

	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			map[sr + r][sc + c] = tmpMAP[size - 1 - c][r];
	// (r, c) <- (size-1-c, r) 가 시계 방향 90도다.

}

RCE rotateRC(RCE rce, RCS squareInfo)
{
	RCE ret = { 0 };
	int tmpMAP[MAX][MAX] = { 0 };

	tmpMAP[rce.r][rce.c] = 1;
	// 좌표 하나만 찍은 빈 격자를 같은 방식으로 돌려서 옮겨진 자리를 찾는다.

	rotate(tmpMAP, squareInfo.r, squareInfo.c, squareInfo.size);

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (tmpMAP[r][c] == 1)
			{
				ret.r = r;
				ret.c = c;

				return ret;
			}
		}
	}

	// for debug
	//ret.r = ret.c = -1;
	return ret;
}

void durabilityDown(RCS squareInfo)
{
	int sr = squareInfo.r;
	int sc = squareInfo.c;
	int er = squareInfo.r + squareInfo.size;
	int ec = squareInfo.c + squareInfo.size;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == 0) continue;
			// 내구도가 0인 칸(빈칸)은 더 깎을 것이 없다.

			if (sr <= r && r < er && sc <= c && c < ec)
				MAP[r][c]--;
		}
	}
}

void rotateMaze()
{
	RCS squareInfo = getSqureInfo();

	rotate(MAP, squareInfo.r, squareInfo.c, squareInfo.size);

	destination = rotateRC(destination, squareInfo);

	for (int m = 0; m < M; m++)
	{
		if (player[m].escape == true) continue;

		player[m] = rotateRC(player[m], squareInfo);
	}

	durabilityDown(squareInfo);
}

void simulate()
{
	int sum = 0;
	for (int k = 0; k < K; k++)
	{
		int step = move();
		sum += step;

		if (check() == true) break;

		rotateMaze();
		//printStatus();

	}

	answerSum = sum;   // [수정] printf -> 전역에 담아 solution()이 반환한다
}

// [수정] main() -> solution().
// 원본은 simulate() 안에서 두 줄을 출력했다. 같은 순서로 세 값을 담아 반환한다.
//   [0] 참가자들의 총 이동 거리, [1] 마지막 출구의 행, [2] 마지막 출구의 열
vector<int> solution(int k, vector<vector<int>> board, vector<vector<int>> players, vector<int> exitPos)
{
	input(k, board, players, exitPos);

	simulate();

	int out[3];
	out[0] = answerSum;   // [수정] printf -> 목록으로 반환
	out[1] = destination.r;
	out[2] = destination.c;

	return vector<int>(out, out + 3);
}
