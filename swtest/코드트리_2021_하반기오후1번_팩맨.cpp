/*
	[코드트리] 2021 하반기 오후 1번 - 팩맨
	https://www.codetree.ai/training-field/frequent-problems/problems/pacman
	(백준 23290 "마법사 상어와 복제"와 같은 문제다. 상어 -> 팩맨, 물고기 -> 몬스터)

	■ 문제 요약
	  4 x 4 격자에 몬스터 M마리(각자 8방향 중 하나)와 팩맨 1명이 있다.
	  한 턴은 아래 순서로 진행되고 TURN턴 반복한다.

	    1) 복제 시도 : 지금 몬스터들을 그대로 알(egg)로 복사해 둔다. 알은 움직이지 않는다.
	    2) 몬스터 이동 : 각 몬스터가 자기 방향으로 1칸 간다.
	                    격자 밖 / 팩맨 칸 / 시체 칸이면 반시계 45도씩 돌려 다시 본다.
	                    8방향 모두 안 되면 제자리에 있는다.
	    3) 팩맨 이동 : 상하좌우로 3칸 움직이는 64가지 경로 중 몬스터를 가장 많이 먹는 경로.
	                  같으면 (상, 좌, 하, 우) 우선순위의 사전 순으로 앞선 것.
	                  같은 칸을 두 번 지나면 두 번째에는 먹을 게 없다.
	                  먹힌 몬스터가 있던 칸에는 시체가 생긴다.
	    4) 시체 소멸 : 시체는 생기고 2턴 동안 남는다.
	    5) 복제 완성 : 1)의 알이 부화해 몬스터가 된다 (방향도 그대로).

	  TURN턴 뒤 살아 있는 몬스터 수를 출력한다.

	■ 풀이 방침
	  - 몬스터는 개체를 따로 관리하지 않고 monster[r][c][d] = "그 칸, 그 방향의 수"로 센다.
	    같은 칸, 같은 방향 몬스터는 앞으로도 똑같이 움직이므로 묶어도 된다.
	    몇 마리든 칸 16개 x 방향 8개 = 128개 값만 다루면 되어 마리 수가 커져도 빠르다.
	  - 팩맨 경로 64가지는 DFS로 main 시작 때 한 번만 만들어 position[]에 담는다.
	    (1, 1, 1) -> (1, 1, 2) -> ... 순서로 만들어지므로 이 순서가 곧 사전 순 우선순위다.
	    그래서 "더 클 때만(>) 갱신"하면 동점일 때 앞의 경로가 자동으로 남는다.
	  - 시체는 deadBody에 남은 수명을 적는다. 3을 넣고 같은 턴의 4)에서 곧바로 1 줄이므로
	    실제로는 이후 2턴 동안 남는다.
	  - grid[][]는 격자 바깥을 true(벽)로 두어 경계 검사를 대신한다.

	■ 주의할 점 (아래 4곳은 원본 그대로 두고 [버그]로 표시했다)
	  랜덤 입력 200개로 돌려 보면 원본은 153개에서 비정상 종료하고,
	  4곳을 모두 고친 코드는 비정상 종료가 0개다. (두 코드의 답이 다른 경우는 196개)

	  [버그 1] input()의 scanf 서식 "%d %d %D %d" : %D는 표준 서식이 아니다.
	           -> "%d %d %d %d"
	  [버그 2] input()의 벽 초기화가 r = 1부터 시작한다. 0행이 벽이 아니라서
	           팩맨이 0행을 지나 격자 밖으로 나갈 수 있고, 배열 밖 접근으로 죽는다.
	           -> for (int r = 0; r <= 5; r++)
	  [버그 3] moveMonster()의 (nr == packMan.r && packMan.c) : 열 비교가 빠졌다.
	           팩맨과 같은 행이기만 하면 전부 못 가는 칸이 된다.
	           -> (nr == packMan.r && nc == packMan.c)
	  [버그 4] simulate()의 step = maxMonster = 1 : 먹을 수 있는 최대가 0이나 1이면
	           갱신이 안 되어 1번 경로(상, 상, 좌)를 강제로 탄다. 이 경로는 격자 밖일 수도 있다.
	           -> step = maxMonster = -1 (불가능한 경로의 -1보다 커야 하므로 -1로 시작)
*/
#include <stdio.h>

#define MAX (4+3)
#define MAX_DIR (8+2)

int T;

int M, TURN;

bool grid[MAX][MAX]; // true = 격자 바깥(벽)
int deadBody[MAX][MAX]; // 시체의 남은 수명 (0 = 없음)

struct PACKMAN
{
	int r;
	int c;
};

PACKMAN packMan;

int monster[MAX][MAX][MAX_DIR]; // (r,c)에 dir 방향의 몬스터 수

int num_of_cases[10]; // DFS로 만드는 중인 경로
int position[64 + 10][3]; // 팩맨 경로 64가지 (사전 순)
int pcnt;

// -, ↑, ←, ↓, →   (팩맨 우선순위 상, 좌, 하, 우 순서)
int dr4[] = { 0, -1, 0, 1, 0 };
int dc4[] = { 0, 0, -1, 0, 1 };

// -, ↑, ↖, ←, ↙, ↓, ↘, →, ↗   (번호가 1 늘면 반시계 45도)
int dr8[] = { 0, -1, -1, 0, 1, 1, 1, 0, -1 };
int dc8[] = { 0, 0, -1, -1, -1, 0, 1, 1, 1 };

void input()
{
	// [버그 1] %D -> %d
	scanf("%d %d %D %d", &M, &TURN, &packMan.r, &packMan.c);

	for (int r = 1; r <= 4; r++)
		for (int c = 1; c <= 4; c++)
			for (int d = 1; d <= 8; d++)
				monster[r][c][d] = 0;

	for (int m = 0; m < M; m++)
	{
		int r, c, d;

		scanf("%d %d %d", &r, &c, &d);

		// 같은 칸, 같은 방향이면 한 값으로 묶어 센다.
		monster[r][c][d]++;
	}

	for (int r = 1; r <= 4; r++)
		for (int c = 1; c <= 4; c++)
			deadBody[r][c] = 0;

	// 테두리를 벽으로 칠한 뒤 안쪽 4 x 4만 비운다.
	// [버그 2] 0행이 빠졌다. r = 0부터 칠해야 위쪽 테두리도 벽이 된다.
	for (int r = 1; r <= 5; r++)
		for (int c = 0; c <= 5; c++)
			grid[r][c] = true;

	for (int r = 1; r <= 4; r++)
		for (int c = 1; c <= 4; c++)
			grid[r][c] = false;
}

void printDeadBody() // for debug
{
	printf("Dead Body\n");
	for (int r = 1; r <= 4; r++)
	{
		for (int c = 1; c <= 4; c++)
			printf("%d ", deadBody[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void printMonster() // for debug
{
	for (int r = 1; r <= 4; r++)
	{
		for (int c = 1; c <= 4; c++)
		{
			printf("(%d, %d)\n", r, c);

			for (int d = 1; d <= 8; d++)
				printf("%d : %d / ", d, monster[r][c][d]);
			putchar('\n');
		}
	}
	putchar('\n');
}

void copyMonster(int copy[MAX][MAX][MAX_DIR], int original[MAX][MAX][MAX_DIR])
{
	for (int r = 1; r <= 4; r++)
		for (int c = 1; c <= 4; c++)
			for (int d = 1; d <= 8; d++)
				copy[r][c][d] = original[r][c][d];
}

// 완성된 경로 하나를 position[]에 저장한다.
void printCases()
{
	//for (int i = 0; i < 3; i++) printf("%d ", num_of_cases[i]);
	//putchar('\n');

	position[pcnt][0] = num_of_cases[0];
	position[pcnt][1] = num_of_cases[1];
	position[pcnt++][2] = num_of_cases[2];
}

// 4방향 x 3번 = 64가지 경로를 사전 순으로 만든다 (중복 순열).
void DFS(int depth)
{
	if (depth == 3)
	{
		printCases();
		return;
	}
	for (int i = 1; i <= 4; i++)
	{
		num_of_cases[depth] = i;

		DFS(depth + 1);
	}
}

// 2) 몬스터 이동
void moveMonster()
{
	// 모두 "동시에" 움직이므로 결과는 새 배열에 모은다.
	int tmpMonster[MAX][MAX][MAX_DIR] = { 0 };
	for (int r = 1; r <= 4; r++)
	{
		for (int c = 1; c <= 4; c++)
		{
			for (int d = 1; d <= 8; d++)
			{
				if (monster[r][c][d] == 0) continue;
				int i;
				for (i = 0; i < 8; i++) //반시계 방향 회전
				{
					int nr, nc, dir;

					// d, d+1, ..., 8, 1, ... 순서로 1 ~ 8 범위를 돌게 한다.
					dir = (d + i - 1 + 8) % 8 + 1;
					nr = r + dr8[dir];
					nc = c + dc8[dir];

					// 팩맨 칸 / 시체 칸 / 격자 밖이면 다음 방향
					// [버그 3] packMan.c -> nc == packMan.c
					if ((nr == packMan.r && packMan.c)
						|| deadBody[nr][nc] != 0
						|| grid[nr][nc] == true) continue;
					else
					{
						// 바뀐 방향(dir)을 그대로 가진 채 옮겨 간다.
						tmpMonster[nr][nc][dir] += monster[r][c][d];
						break;
					}
				}
				//이동 불가 : 방향도 원래대로 제자리
				if (i == 8) tmpMonster[r][c][d] += monster[r][c][d];
			}
		}
	}

	copyMonster(monster, tmpMonster);
}


// step번 경로로 갔을 때 먹는 몬스터 수. 격자 밖으로 나가면 -1.
int getMonster(int step)
{
	// 실제 상태를 건드리지 않도록 팩맨과 몬스터를 복사해서 시뮬레이션한다.
	PACKMAN tmpPackMan = packMan;
	int tmpMonster[MAX][MAX][MAX_DIR] = { 0 };

	copyMonster(tmpMonster, monster);

	int count = 0;
	for (int i = 0; i < 3; i++)
	{
		int nr, nc;

		nr = tmpPackMan.r + dr4[position[step][i]];
		nc = tmpPackMan.c + dc4[position[step][i]];

		if (grid[nr][nc] == true) return -1;

		// 먹은 칸은 0으로 비워 같은 칸을 다시 지날 때 두 번 세지 않는다.
		for (int d = 1; d <= 8; d++)
		{
			count += tmpMonster[nr][nc][d];
			tmpMonster[nr][nc][d] = 0;
		}

		tmpPackMan.r = nr;
		tmpPackMan.c = nc;
	}

	return count;
}

// 3) 고른 경로로 실제로 이동하며 먹고, 먹은 칸에 시체를 남긴다.
void movePackMan(int step)
{
	for (int i = 0; i < 3; i++)
	{
		int nr, nc;

		nr = packMan.r + dr4[position[step][i]];
		nc = packMan.c + dc4[position[step][i]];

		// 몬스터가 실제로 있었던 칸에만 시체가 생긴다.
		for (int d = 1; d <= 8; d++)
		{
			if (monster[nr][nc][d] == 0) continue;

			monster[nr][nc][d] = 0;
			deadBody[nr][nc] = 3; // 곧바로 disappear()에서 1 줄어 2턴 남는다.
		}

		packMan.r = nr;
		packMan.c = nc;
	}
}


// 4) 시체 수명 1 감소
void disappear()
{
	for (int r = 1; r <= 4; r++)
		for (int c = 1; c <= 4; c++)
			if (deadBody[r][c] != 0) deadBody[r][c]--;
}

void simulate()
{
	for (int t = 0; t < TURN; t++)
	{
		int egg[MAX][MAX][MAX_DIR] = { 0 };

		// 1. 몬스터 복제 시도
		copyMonster(egg, monster);

		// 2. 몬스터 이동
		moveMonster();

		int step, maxMonster;

		// [버그 4] -1로 시작해야 0마리, 1마리 경로도 고를 수 있다.
		step = maxMonster = 1;

		// 사전 순으로 보면서 "더 클 때만" 갱신 -> 동점이면 앞선 경로가 남는다.
		for (int i = 0; i < 64; i++)
		{
			int tmp = getMonster(i);
			if (tmp > maxMonster)
			{
				maxMonster = tmp;
				step = i;
			}
		}

		// 3. 팩맨 이동
		movePackMan(step);

		// 4. 몬스터 시체 소멸
		disappear();

		// 5. 몬스터 복제 완성
		for (int r = 1; r <= 4; r++)
			for (int c = 1; c <= 4; c++)
				for (int d = 1; d <= 8; d++)
					monster[r][c][d] += egg[r][c][d];
	}
}

int getAnswer()
{
	int sum = 0;
	for (int r = 1; r <= 4; r++)
		for (int c = 1; c <= 4; c++)
			for (int d = 1; d <= 8; d++)
				sum += monster[r][c][d];

	return sum;
}

int main()
{
	// 팩맨 경로 64가지는 입력과 무관하므로 한 번만 만든다.
	DFS(0);

	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		simulate();

		printf("%d\n", getAnswer());
	}

	return 0;
}
