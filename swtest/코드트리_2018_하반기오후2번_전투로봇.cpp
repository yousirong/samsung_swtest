/*
	[코드트리] 2018 하반기 오후 2번 - 전투로봇
	https://www.codetree.ai/training-field/frequent-problems/problems/fighting-robot

	■ 문제 요약
	  N x N 격자에 전투로봇 하나(입력에서 9로 표시)와 몬스터들이 있다.
	  칸의 숫자는 그 자리에 있는 몬스터의 크기이고 0은 빈칸이다.

	  로봇은 레벨 2로 시작하고, 상하좌우로 한 칸씩 1초에 이동한다.

	    자기 레벨보다 작은 몬스터 : 지나갈 수 있고, 제거할 수도 있다
	    자기 레벨과 같은 몬스터   : 지나갈 수는 있지만 제거는 못 한다
	    자기 레벨보다 큰 몬스터   : 지나갈 수 없다

	  갈 수 있는 몬스터가 여러 마리면 아래 순서로 하나를 고른다.
	    1) 가장 가까운 몬스터
	    2) 거리가 같으면 가장 위쪽
	    3) 그것도 같으면 가장 왼쪽

	  현재 레벨과 같은 수만큼 몬스터를 제거하면 레벨이 1 오르고 제거 횟수는 0이 된다.
	  더 이상 제거할 몬스터가 없을 때까지 걸린 총 이동 시간을 출력한다.

	  (백준 16236 "아기 상어"와 같은 문제다)

	■ 풀이 방침 : "한 마리 잡을 때마다 BFS를 다시"
	  로봇의 레벨이 바뀌면 지나갈 수 있는 칸도, 잡을 수 있는 대상도 달라진다.
	  그래서 경로를 미리 계산해 둘 수 없고, 한 마리 잡을 때마다 처음부터 다시 재야 한다.

	    1) 지금 레벨로 잡을 수 있는 몬스터가 남아 있는지 확인한다. 없으면 끝.
	    2) 현재 위치에서 BFS로 모든 칸까지의 최단 거리를 구한다.
	    3) 잡을 수 있는 몬스터 중 우선순위에 맞는 하나를 고른다.
	    4) 그 자리로 이동해 몬스터를 지우고, 시간과 레벨을 갱신한다.
	    5) 1번으로 돌아간다.

	■ 우선순위를 따로 정렬하지 않는 이유
	  후보를 고르는 이중 for문이 r을 작은 값부터, 그 안에서 c를 작은 값부터 훑는다.
	  그리고 갱신 조건이 "거리가 더 작을 때만"(<)이라 같은 거리면 먼저 만난 쪽이 남는다.
	  훑는 순서가 곧 위 -> 아래, 왼쪽 -> 오른쪽이므로
	  "가까운 것 > 위쪽 > 왼쪽" 우선순위가 별도 처리 없이 그대로 지켜진다.

	■ BFS에서 유의할 점
	  이동 가능 조건은 "내 레벨보다 큰 몬스터가 아닐 것"뿐이다.
	  같은 레벨 몬스터가 있는 칸은 잡지는 못해도 통과는 가능하므로 막으면 안 된다.

	  visit이 방문 여부와 거리를 겸하고, 0을 미방문으로 쓰기 때문에 시작 칸을 1로 둔다.
	  그래서 실제 이동 시간은 visit 값에서 1을 뺀 값이다.

	■ 반환값 0을 종료 신호로 쓰는 것에 대해
	  로봇이 서 있는 칸은 빈칸이라 후보가 될 수 없고, 후보의 visit 값은 항상 2 이상이다.
	  따라서 실제 이동 시간(minTime - 1)이 0이 되는 경우는 없고,
	  0은 "더 잡을 게 없다"는 뜻으로만 쓰이므로 신호가 겹치지 않는다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (20 + 5)
#define INF (0x7fff0000)

#define ROBOT_POSITION (9)

int T;

int N;
int MAP[MAX][MAX];     // 칸의 몬스터 크기 (0이면 빈칸)
int visit[MAX][MAX];   // BFS 방문 여부 + 거리 (0이면 미방문/도달 불가)

// 로봇 상태
struct ROBOT
{
	int r;
	int c;
	int attack;   // 현재 레벨에서 제거한 몬스터 수
	int level;    // 현재 레벨
};

typedef struct ROBOT ROBOT;

ROBOT battleRobot;

// 좌표 한 쌍
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX * MAX];   // BFS용 큐

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d", &N);

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%d", &MAP[r][c]);

			if (MAP[r][c] == ROBOT_POSITION)
			{
				// 로봇이 서 있는 칸에는 몬스터가 없으므로 빈칸으로 바꿔 둔다.
				// 이렇게 해야 이후 판정에서 자기 자신을 몬스터로 오인하지 않는다.
				MAP[r][c] = 0;

				battleRobot.r = r;
				battleRobot.c = c;
				battleRobot.attack = 0;
				battleRobot.level = 2;
			}
		}
	}
}

// ---------------------------
// 디버그용: 거리 배열 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", visit[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 지금 레벨로 잡을 수 있는 몬스터가 격자에 남아 있는지
//
// (도달 가능한지는 보지 않는다. 그건 BFS 뒤에 걸러진다)
// ---------------------------
bool monsterExists()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] != 0 && MAP[r][c] < battleRobot.level)
				return true;
		}
	}

	return false;
}

// ---------------------------
// 현재 로봇 위치에서 모든 칸까지의 최단 거리 계산
//
//   visit == 0     : 미방문(= 도달 불가)
//   visit == 1     : 시작 칸
//   visit == k     : 시작 칸에서 k-1번 이동하면 닿는 칸
// ---------------------------
void BFS()
{
	// 레벨이 바뀌면 통과 가능한 칸도 달라지므로 매번 새로 계산한다
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			visit[r][c] = 0;
		}
	}

	int rp, wp;
	rp = wp = 0;

	queue[wp].r = battleRobot.r;
	queue[wp++].c = battleRobot.c;

	visit[battleRobot.r][battleRobot.c] = 1;   // 0은 미방문이라 1부터 센다

	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr = out.r + dr[i];
			int nc = out.c + dc[i];

			// 격자 밖
			if (nr < 1 || nc < 1 || nr > N || nc > N)
				continue;

			// 나보다 큰 몬스터가 있는 칸만 막힌다.
			// 같은 레벨 몬스터 칸은 잡지는 못해도 통과는 된다.
			if (battleRobot.level < MAP[nr][nc])
				continue;

			// 이미 더 짧은 경로로 도달한 칸
			if (visit[nr][nc] != 0)
				continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = visit[out.r][out.c] + 1;
		}
	}
}

// ---------------------------
// 몬스터 한 마리를 잡는 한 단계
//
//   반환값 : 이번에 이동한 시간. 더 잡을 수 없으면 0.
// ---------------------------
int simulate()
{
	// 격자에 잡을 수 있는 몬스터 자체가 없으면 끝
	if (monsterExists() == false)
		return 0;

	BFS();

	int tmpR, tmpC;
	int minTime = INF;

	// 후보 고르기.
	// 위 -> 아래, 왼쪽 -> 오른쪽 순으로 훑고 "더 가까울 때만" 갱신하므로
	// 거리 > 위쪽 > 왼쪽 우선순위가 자동으로 지켜진다.
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			// 빈칸이거나 아예 도달할 수 없는 칸
			if (MAP[r][c] == 0 || visit[r][c] == 0)
				continue;

			// 나와 같거나 큰 몬스터는 잡을 수 없다
			if (battleRobot.level <= MAP[r][c])
				continue;

			if (visit[r][c] < minTime)
			{
				minTime = visit[r][c];
				tmpR = r;
				tmpC = c;
			}
		}
	}

	// printMap();

	// 잡을 몬스터가 남아 있긴 하지만 벽에 막혀 갈 수 없는 경우
	if (minTime == INF)
		return 0;

	// ----- 몬스터 제거와 상태 갱신 -----

	MAP[tmpR][tmpC] = 0;        // 몬스터가 있던 칸은 빈칸이 된다

	battleRobot.r = tmpR;       // 로봇이 그 자리로 이동
	battleRobot.c = tmpC;

	battleRobot.attack++;

	// 현재 레벨만큼 잡으면 레벨업하고 카운터를 초기화한다
	if (battleRobot.attack == battleRobot.level)
	{
		battleRobot.level++;
		battleRobot.attack = 0;
	}

	// visit은 시작 칸을 1로 잡았으므로 실제 이동 시간은 -1
	return minTime - 1;
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		int time = 0;   // 누적 이동 시간

		while (1)
		{
			int result = simulate();

			// 0이면 더 잡을 몬스터가 없다는 뜻
			if (result == 0)
				break;

			time += result;
		}

		printf("%d\n", time);
	}

	return 0;
}
