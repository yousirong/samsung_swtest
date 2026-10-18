/*
	[SWEA] 5650 - [모의 SW 역량테스트] 핀볼 게임
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AWXRF8s6ezEDFAUo

	■ 문제 요약
	  N x N 판(5 <= N <= 100)에 빈 칸(0), 블록(1 ~ 5), 웜홀(6 ~ 10, 같은 번호끼리 한 쌍),
	  블랙홀(-1)이 있다. 판 바깥은 벽이다.
	  빈 칸 하나에서 상하좌우 중 한 방향으로 공을 출발시킨다.
	    - 블록이나 벽에 부딪히면 점수 +1 하고 방향이 바뀐다
	      (블록 1 ~ 4는 직각삼각형이라 빗변이면 90도 꺾이고, 평평한 면이면 반사. 5와 벽은 전부 반사)
	    - 웜홀에 들어가면 같은 번호의 다른 웜홀로 나와 같은 방향으로 계속 간다
	    - 블랙홀에 빠지거나 출발 위치로 돌아오면 게임 끝
	  얻을 수 있는 점수의 최댓값을 구한다. 테스트 케이스가 T개.

	■ 풀이 방침 : 모든 빈 칸 x 4방향을 전부 시뮬레이션
	  공이 가는 길은 정해져 있어서 고를 것이 없다. 출발점과 방향만 바꿔 가며 다 돌려 본다.

	■ 블록 처리 : "어느 면으로 들어왔나"로 표를 만든다
	  MAP[r][c].direction[면] = 그 면으로 들어온 공이 어떻게 되는지
	      REFLECT      : 왔던 방향으로 되돌아간다
	      CHANGE_xxx   : xxx 방향으로 꺾인다
	  공이 LEFT로 움직이면 블록의 RIGHT 면에 부딪힌다 -> checkDir[진행 방향] = 부딪히는 면 (반대 방향)
	  changeDir[진행 방향][결과] = 새 방향 (REFLECT면 반대 방향, CHANGE_xxx면 xxx)
	  changeDir의 -1 칸은 나올 수 없는 조합이다 (예: 왼쪽으로 가다 왼쪽으로 꺾임).

	      블록 1 (◣) : 오른쪽 면 -> 위로,     위쪽 면 -> 오른쪽으로,  나머지 반사
	      블록 2 (◤) : 오른쪽 면 -> 아래로,   아래쪽 면 -> 오른쪽으로, 나머지 반사
	      블록 3 (◥) : 왼쪽 면 -> 아래로,     아래쪽 면 -> 왼쪽으로,   나머지 반사
	      블록 4 (◢) : 왼쪽 면 -> 위로,       위쪽 면 -> 왼쪽으로,     나머지 반사
	      블록 5 (■) : 전부 반사

	■ 벽 = 블록 5로 두른다
	  테두리(0행, N+1행, 0열, N+1열)를 type 5(사방 반사)로 채워 두면 벽도 블록과 똑같이 처리된다.
	  공은 테두리 칸에 들어갔다가 방향만 바뀌어 다시 안으로 나온다. 범위 검사가 필요 없다.

	■ 주의할 점
	  - 출발 위치로 "돌아오는" 순간 끝난다. 끝까지 갔다 오는 게 아니라 지나가기만 해도 끝이다.
	    그래서 한 칸 움직일 때마다 (sr, sc)와 비교한다.
	  - 블록에 부딪혀도 공은 그 블록 칸에 있다고 보고, 다음 이동에서 바뀐 방향으로 나간다.
	  - 웜홀은 칸 위치만 바꾸고 방향은 그대로다. 같은 번호의 두 칸 중 "지금 칸이 아닌 쪽"으로 간다.
	  - wcnt[]는 케이스마다 비운다. 테두리 type도 케이스마다 다시 채운다 (N이 바뀌므로).
	  - printMap()은 디버그용인데 오타가 두 개 있다 (호출은 주석 처리돼 있어 결과에는 영향 없음).
	      "ball r : &d" -> "%d",   ball.r == c -> ball.c == c
	  - simulate() 끝의 "impossible case!"는 while(1)을 빠져나오는 길이 없어 실행되지 않는다.
*/
#include <stdio.h>

#define MAX (100+20)

#define BLACKHOLE (-1)
#define EMPTY (0)
#define BLOCK_START (1)
#define BLOCK_END (5)
#define WORMHOLE_START (6)
#define WORMHOLE_END (10)

#define LEFT (0)
#define UP (1)
#define RIGHT (2)
#define DOWN (3)

// 블록 면에 부딪혔을 때 결과
#define REFLECT (0)
#define CHANGE_LEFT (1)
#define CHANGE_UP (2)
#define CHANGE_RIGHT (3)
#define CHANGE_DOWN (4)

int T;
int N;

// 왼쪽, 위, 오른쪽, 아래
int dr[] = { 0,-1,0,1 };
int dc[] = { -1,0,1,0 };

int changeDir[4][5];   // changeDir[진행 방향][결과] = 새 방향

struct BLOCK
{
	int direction[4];  // direction[면] : 그 면에 부딪혔을 때 결과 (REFLECT / CHANGE_xxx)
	int type;          // 칸 종류 (-1 ~ 10)
};

BLOCK MAP[MAX][MAX];

struct BALL
{
	int r;
	int c;
	int dir;
};

BALL ball;

struct WORMHOLE
{
	int r;
	int c;
};

WORMHOLE wormhole[10 + 2][2];   // wormhole[번호][0 / 1] : 같은 번호 웜홀 두 칸
int wcnt[10 + 2];

void input()
{
	scanf("%d", &N);

	for (int w = 6; w <= 10; w++) wcnt[w] = 0;

	// 테두리 포함 전부 블록 5(사방 반사)로 -> 벽 역할
	for (int r = 0; r <= N + 1; r++)
	{
		for (int c = 0; c <= N + 1; c++)
		{
			MAP[r][c].type = 5;
			MAP[r][c].direction[LEFT] = REFLECT;
			MAP[r][c].direction[UP] = REFLECT;
			MAP[r][c].direction[RIGHT] = REFLECT;
			MAP[r][c].direction[DOWN] = REFLECT;

		}
	}

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			int type;

			scanf("%d", &type);

			MAP[r][c].type = type;

			if (type == 1)        // ◣
			{
				MAP[r][c].direction[LEFT] = REFLECT;
				MAP[r][c].direction[UP] = CHANGE_RIGHT;
				MAP[r][c].direction[RIGHT] = CHANGE_UP;
				MAP[r][c].direction[DOWN] = REFLECT;
			}
			else if (type == 2)   // ◤
			{
				MAP[r][c].direction[LEFT] = REFLECT;
				MAP[r][c].direction[UP] = REFLECT;
				MAP[r][c].direction[RIGHT] = CHANGE_DOWN;
				MAP[r][c].direction[DOWN] = CHANGE_RIGHT;
			}
			else if (type == 3)   // ◥
			{
				MAP[r][c].direction[LEFT] = CHANGE_DOWN;
				MAP[r][c].direction[UP] = REFLECT;
				MAP[r][c].direction[RIGHT] = REFLECT;
				MAP[r][c].direction[DOWN] = CHANGE_LEFT;
			}
			else if (type == 4)   // ◢
			{
				MAP[r][c].direction[LEFT] = CHANGE_UP;
				MAP[r][c].direction[UP] = CHANGE_LEFT;
				MAP[r][c].direction[RIGHT] = REFLECT;
				MAP[r][c].direction[DOWN] = REFLECT;
			}
			else if (type == 5)   // ■
			{
				MAP[r][c].direction[LEFT] = REFLECT;
				MAP[r][c].direction[UP] = REFLECT;
				MAP[r][c].direction[RIGHT] = REFLECT;
				MAP[r][c].direction[DOWN] = REFLECT;
			}
			else if (WORMHOLE_START <= type && type <= WORMHOLE_END)   // 웜홀 위치 저장
			{
				wormhole[type][wcnt[type]].r = r;
				wormhole[type][wcnt[type]++].c = c;
			}
		}
	}
}

void printMap()   // for debug (오타 2개 : "&d" -> "%d", ball.r == c -> ball.c == c)
{
	printf("ball r : &d, c : %d, dir %d\n", ball.r, ball.c, ball.dir);
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (ball.r == r && ball.r == c) printf("[] ");
			else printf("%2d ", MAP[r][c].type);
		}
		putchar('\n');
	}
	putchar('\n');
}


// (sr, sc)에서 dir 방향으로 출발했을 때 점수
int simulate(int sr, int sc, int dir)
{
	int checkDir[4];   // checkDir[진행 방향] = 블록에서 부딪히는 면 (반대 방향)

	checkDir[LEFT] = RIGHT;
	checkDir[RIGHT] = LEFT;
	checkDir[UP] = DOWN;
	checkDir[DOWN] = UP;

	ball.r = sr, ball.c = sc;
	ball.dir = dir;

	int score = 0;

	while (1)
	{
		//printMap();

		int nr, nc, mapType;

		// 한 칸 이동
		nr = ball.r + dr[ball.dir];
		nc = ball.c + dc[ball.dir];

		ball.r = nr;
		ball.c = nc;

		mapType = MAP[nr][nc].type;

		// 블랙홀이거나 출발 위치로 돌아오면 끝
		if (mapType == BLACKHOLE || (ball.r == sr && ball.c == sc)) return score;

		// 블록 / 벽 : 점수 +1, 부딪힌 면에 따라 방향을 바꾼다
		if (BLOCK_START <= mapType && mapType <= BLOCK_END)
		{
			score++;
			ball.dir = changeDir[ball.dir][MAP[nr][nc].direction[checkDir[ball.dir]]];

		}
		// 웜홀 : 짝이 되는 다른 웜홀로 순간이동 (방향은 그대로)
		else if (WORMHOLE_START <= mapType && mapType <= WORMHOLE_END)
		{
			if (wormhole[mapType][0].r == nr && wormhole[mapType][0].c == nc)
			{
				ball.r = wormhole[mapType][1].r;
				ball.c = wormhole[mapType][1].c;
			}
			else
			{
				ball.r = wormhole[mapType][0].r;
				ball.c = wormhole[mapType][0].c;
			}
		}
	}

	printf("impossible case!\n");
	return -1; // impossible case
}

int main()
{
	// changeDir[진행 방향][결과] = 새 방향. -1은 나올 수 없는 조합
	changeDir[LEFT][REFLECT] = RIGHT;
	changeDir[LEFT][CHANGE_LEFT] = -1;
	changeDir[LEFT][CHANGE_UP] = UP;
	changeDir[LEFT][CHANGE_RIGHT] = -1;
	changeDir[LEFT][CHANGE_DOWN] = DOWN;

	changeDir[UP][REFLECT] = DOWN;
	changeDir[UP][CHANGE_LEFT] = LEFT;
	changeDir[UP][CHANGE_UP] = -1;
	changeDir[UP][CHANGE_RIGHT] = RIGHT;
	changeDir[UP][CHANGE_DOWN] = -1;

	changeDir[RIGHT][REFLECT] = LEFT;
	changeDir[RIGHT][CHANGE_LEFT] = -1;
	changeDir[RIGHT][CHANGE_UP] = UP;
	changeDir[RIGHT][CHANGE_RIGHT] = -1;
	changeDir[RIGHT][CHANGE_DOWN] = DOWN;

	changeDir[DOWN][REFLECT] = UP;
	changeDir[DOWN][CHANGE_LEFT] = LEFT;
	changeDir[DOWN][CHANGE_UP] = -1;
	changeDir[DOWN][CHANGE_RIGHT] = RIGHT;
	changeDir[DOWN][CHANGE_DOWN] = -1;

	scanf("%d", &T);

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		int MAXANS = 0;

		// 모든 빈 칸 x 4방향
		for (int r = 1; r <= N; r++)
		{
			for (int c = 1; c <= N; c++)
			{
				if (MAP[r][c].type != 0) continue;   // 빈 칸에서만 출발

				for (int dir = 0; dir < 4; dir++)
				{
					int score = simulate(r, c, dir);
					if (MAXANS < score) MAXANS = score;
				}
			}
		}
		printf("#%d %d\n", tc, MAXANS);
	}
	return 0;

}
