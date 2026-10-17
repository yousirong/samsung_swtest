/*
	[코드트리] 2026 하반기 오전 1번 - 수하물 컨베이어 벨트
	https://www.codetree.ai/ko/frequent-problems/samsung-sw/problems/baggage-conveyor-belt/description

	■ 문제 요약
	  N x N 격자(2 <= N <= 8). 0 벽, 1 ~ 4 레일(1 아래, 2 왼쪽, 3 위, 4 오른쪽), 9 적재 장소.
	  어느 레일에서 출발해도 화살표를 따라가면 반드시 적재 장소에 닿는다 (순환이 없다).
	  수하물 B개(최대 30). i번은 무게 w_i, t_i턴에 (r_i, c_i)로 투입될 예정이다.
	  매 턴
	    1단계 이동 : 벨트 위 모든 수하물이 동시에 화살표 방향으로 한 칸 가려고 한다.
	       - 같은 칸으로 여럿이 가려 하면 가장 무거운 것(같으면 번호가 작은 것)만 갈 수 있다
	       - 가려는 칸에 수하물이 있고 그 수하물이 이번 턴에 못 움직이면 나도 못 움직인다
	         (앞 수하물이 움직이면 비운 자리로 들어갈 수 있다)
	    2단계 적재 / 파손 : 적재 장소에 도착한 수하물은 적재 (그 턴 번호 기록).
	       남은 수하물 중 3턴 연속 못 움직인 것은 파손 표시 (그래도 계속 움직이고 적재된다).
	    3단계 투입 : 예정 턴이 됐거나 지난 미투입 수하물이 투입 칸에 올라가려 한다.
	       칸이 차 있으면 실패, 같은 칸에 여럿이면 예정 턴이 가장 이른 것만. 실패하면 다음 턴에 다시.
	  수하물마다 적재 턴을 출력한다. 파손된 것은 -(적재 턴).

	  입출력 예
	      예제 1 -> 5 6 7 -8 -9 10
	      예제 2 -> 7 3 10 6 13 -12 8 11 9 6

	■ 풀이 방침 : 턴마다 3단계를 그대로 시뮬레이션
	  bagMAP[r][c] = 그 칸에 있는 수하물 번호 (0 = 빈 칸) 으로 칸 <-> 수하물을 바로 찾는다.

	■ 1단계의 핵심 : "누가 움직이나"를 먼저 다 정하고, 그다음 한꺼번에 옮긴다
	  1) 칸마다 그 칸으로 가려는 수하물 중 우선순위가 가장 높은 하나(winner)를 정한다
	  2) canMove(i) : i가 이번 턴에 움직일 수 있나 (재귀 + 메모)
	       - 가려는 칸의 winner가 내가 아니면 -> 못 움직임
	       - 가려는 칸이 비어 있으면 -> 움직임
	       - 가려는 칸에 수하물 j가 있으면 -> canMove(j)를 따른다 (j가 비켜 주면 들어간다)
	     화살표에 순환이 없으므로 이 재귀는 반드시 끝난다 (결국 적재 장소나 빈 칸에 닿는다).
	  3) 움직이는 수하물을 전부 bagMAP에서 지운 뒤, 새 칸에 한꺼번에 놓는다
	     (하나씩 옮기면 아직 안 옮긴 앞 수하물 칸을 덮어쓴다)

	■ 주의할 점
	  - 이번 턴에 투입된 수하물은 다음 턴부터 움직인다 -> 투입은 이동 / 적재 뒤(3단계)에 한다.
	  - 파손은 표시만 한다. 무게, 이동 규칙은 그대로다.
	  - 적재 장소(9)에는 수하물이 머무르지 않는다 (도착한 턴에 바로 적재되어 사라진다).
	    그래도 같은 적재 장소로 동시에 둘이 가려 하면 winner 규칙이 적용된다.
	  - 투입 예정 (t, r, c)가 모두 같은 수하물은 없으므로, 같은 칸 투입 경쟁에서 예정 턴이 같은 경우는 없다.
	  - 출력은 한 줄에 공백으로 구분한다.
*/
#include <stdio.h>

#define MAX (8+2)
#define MAX_BAG (30+5)

#define WALL (0)
#define LOAD (9)

#define WAIT (0)    // 아직 투입 안 됨
#define BELT (1)    // 벨트 위
#define DONE (2)    // 적재됨

int T;
int N, B;
int MAP[MAX][MAX];        // 0 벽, 1 ~ 4 레일, 9 적재 장소
int bagMAP[MAX][MAX];     // 그 칸의 수하물 번호 (0 = 빈 칸)

struct BAG
{
	int t, r, c, w;       // 투입 예정 턴, 투입 칸, 무게
	int state;            // WAIT / BELT / DONE
	int stuck;            // 연속으로 못 움직인 턴 수
	int broken;           // 파손 여부
	int result;           // 적재 턴 (파손이면 음수)
};

BAG bag[MAX_BAG];

// -, 아래, 왼쪽, 위, 오른쪽  (레일 번호 1 ~ 4 그대로)
int dr[] = { 0,1,0,-1,0 };
int dc[] = { 0,0,-1,0,1 };

int winner[MAX][MAX];     // 이번 턴에 그 칸으로 갈 수 있는 수하물 (0 = 없음)
int moveState[MAX_BAG];   // canMove 메모 : -1 모름, 0 못 움직임, 1 움직임
int moved[MAX_BAG];       // 이번 턴에 움직였나

void input()
{
	scanf("%d", &N);

	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= N + 1; c++)
			MAP[r][c] = WALL, bagMAP[r][c] = 0;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);

	scanf("%d", &B);

	for (int i = 1; i <= B; i++)
	{
		scanf("%d %d %d %d", &bag[i].t, &bag[i].r, &bag[i].c, &bag[i].w);

		bag[i].state = WAIT;
		bag[i].stuck = 0;
		bag[i].broken = 0;
		bag[i].result = 0;
	}
}

// a가 b보다 우선인가 (무거운 쪽, 같으면 번호가 작은 쪽)
int isPriority(int a, int b)
{
	if (bag[a].w != bag[b].w) return bag[a].w > bag[b].w;
	return a < b;
}

// i번 수하물이 이번 턴에 움직일 수 있나
int canMove(int i)
{
	if (moveState[i] != -1) return moveState[i];

	int nr = bag[i].r + dr[MAP[bag[i].r][bag[i].c]];
	int nc = bag[i].c + dc[MAP[bag[i].r][bag[i].c]];

	if (winner[nr][nc] != i) return moveState[i] = 0;    // 같은 칸 경쟁에서 짐
	if (bagMAP[nr][nc] == 0) return moveState[i] = 1;    // 빈 칸
	return moveState[i] = canMove(bagMAP[nr][nc]);       // 앞 수하물이 비켜 주면 들어간다
}

// 1단계 : 이동
void moveBags()
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			winner[r][c] = 0;

	// 칸마다 가장 우선인 수하물을 정한다
	for (int i = 1; i <= B; i++)
	{
		moveState[i] = -1;
		moved[i] = 0;

		if (bag[i].state != BELT) continue;

		int nr = bag[i].r + dr[MAP[bag[i].r][bag[i].c]];
		int nc = bag[i].c + dc[MAP[bag[i].r][bag[i].c]];

		if (winner[nr][nc] == 0 || isPriority(i, winner[nr][nc])) winner[nr][nc] = i;
	}

	// 누가 움직이는지 먼저 다 정한다
	for (int i = 1; i <= B; i++)
		if (bag[i].state == BELT) moved[i] = canMove(i);

	// 움직이는 수하물을 전부 지운 뒤
	for (int i = 1; i <= B; i++)
		if (moved[i]) bagMAP[bag[i].r][bag[i].c] = 0;

	// 새 칸에 한꺼번에 놓는다
	for (int i = 1; i <= B; i++)
	{
		if (moved[i] == 0) continue;

		int d = MAP[bag[i].r][bag[i].c];
		bag[i].r += dr[d];
		bag[i].c += dc[d];

		bagMAP[bag[i].r][bag[i].c] = i;
	}
}

// 2단계 : 적재, 파손
void loadAndCheck(int turn)
{
	for (int i = 1; i <= B; i++)
	{
		if (bag[i].state != BELT) continue;

		if (MAP[bag[i].r][bag[i].c] == LOAD)   // 적재
		{
			bag[i].state = DONE;
			bag[i].result = bag[i].broken ? -turn : turn;
			bagMAP[bag[i].r][bag[i].c] = 0;
			continue;
		}

		if (moved[i]) bag[i].stuck = 0;
		else
		{
			bag[i].stuck++;
			if (bag[i].stuck >= 3) bag[i].broken = 1;   // 3턴 연속 못 움직임
		}
	}
}

// 3단계 : 투입
void insertBags(int turn)
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (bagMAP[r][c] != 0) continue;   // 칸이 차 있으면 아무도 못 들어간다

			// 이 칸에 들어오려는 수하물 중 예정 턴이 가장 이른 것
			int best = 0;
			for (int i = 1; i <= B; i++)
			{
				if (bag[i].state != WAIT || bag[i].t > turn) continue;
				if (bag[i].r != r || bag[i].c != c) continue;

				if (best == 0 || bag[i].t < bag[best].t) best = i;
			}

			if (best == 0) continue;

			bag[best].state = BELT;
			bag[best].stuck = 0;
			bagMAP[r][c] = best;
		}
	}
}

int isFinished()
{
	for (int i = 1; i <= B; i++)
		if (bag[i].state != DONE) return 0;
	return 1;
}

void simulate()
{
	for (int turn = 1; isFinished() == 0; turn++)
	{
		moveBags();
		loadAndCheck(turn);
		insertBags(turn);
	}
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		simulate();

		for (int i = 1; i <= B; i++)
			printf("%d ", bag[i].result);
		putchar('\n');
	}

	return 0;
}
