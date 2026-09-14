/*
	[프로그래머스 함수형] 2021 하반기 오후 1번 - 팩맨
	원본 : swtest/코드트리_2021_하반기오후1번_팩맨.cpp
	복습 노트 : 없음 (docs/INDEX.md 미등록)

	원본은 stdin에서 scanf로 읽고 printf로 출력한다.
	여기서는 채점기가 값을 인자로 넘겨주고 solution()의 반환값을 비교하는 형태로 바꿨다.

	바뀐 곳은 세 군데뿐이고 전부 // [수정] / // [추가] 표시를 달았다.
	로직 함수(moveMonster, getMonster, movePackMan, disappear, simulate, getAnswer)는 한 줄도 건드리지 않았다.

	[주의] 원본에 버그가 3개 있고 그대로 옮겨 왔다. README.md의 "원본 버그" 절을 볼 것.
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 인자용

#define MAX (4+3)
#define MAX_DIR (8+2)

int T;

int M, TURN;

bool grid[MAX][MAX];
int deadBody[MAX][MAX];

struct PACKMAN
{
	int r;
	int c;
};

PACKMAN packMan;

int monster[MAX][MAX][MAX_DIR]; // (r,c)에 dir 방향의 몬스터 수

int num_of_cases[10];
int position[64 + 10][3];
int pcnt;

// -, ↑, ←, ↓, →
int dr4[] = { 0, -1, 0, 1, 0 };
int dc4[] = { 0, 0, -1, 0, 1 };

// -, ↑, ↖, ←, ↙, ↓, ↘, →, ↗
int dr8[] = { 0, -1, -1, 0, 1, 1, 1, 0, -1 };
int dc8[] = { 0, 0, -1, -1, -1, 0, 1, 1, 1 };

// [수정] scanf 대신 인자로 받는다. 전역 초기화 루프는 원본 그대로 둔다.
void input(int t, int pr, int pc, const std::vector<std::vector<int>>& monsters)
{
	M = (int)monsters.size();   // [수정] scanf 대체 - 마리 수는 배열 길이로 알 수 있다
	TURN = t;                   // [수정] scanf 대체
	packMan.r = pr;             // [수정] scanf 대체
	packMan.c = pc;             // [수정] scanf 대체

	for (int r = 1; r <= 4; r++)
		for (int c = 1; c <= 4; c++)
			for (int d = 1; d <= 8; d++)
				monster[r][c][d] = 0;

	for (int m = 0; m < M; m++)
	{
		int r, c, d;

		r = monsters[m][0];     // [수정] scanf 대체
		c = monsters[m][1];     // [수정] scanf 대체
		d = monsters[m][2];     // [수정] scanf 대체

		monster[r][c][d]++;
	}

	for (int r = 1; r <= 4; r++)
		for (int c = 1; c <= 4; c++)
			deadBody[r][c] = 0;

	// [버그수정] 원본은 r = 1부터라 0행이 벽으로 막히지 않았다.
	//            몬스터와 팩맨이 격자 위로 빠져나가 결국 배열 밖을 건드린다(세그폴트 확인).
	for (int r = 0; r <= 5; r++)
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

void printCases()
{
	//for (int i = 0; i < 3; i++) printf("%d ", num_of_cases[i]);
	//putchar('\n');

	position[pcnt][0] = num_of_cases[0];
	position[pcnt][1] = num_of_cases[1];
	position[pcnt++][2] = num_of_cases[2];
}

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

void moveMonster()
{
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

					dir = (d + i - 1 + 8) % 8 + 1;
					nr = r + dr8[dir];
					nc = c + dc8[dir];

					// [버그수정] 원본은 nc == packMan.c 가 아니라 packMan.c 를 그대로 참이냐로 봤다.
					//            packMan.c 는 1~4라 항상 참이 되어, 행만 같으면 막힌 것으로 오판한다.
					if ((nr == packMan.r && nc == packMan.c)
						|| deadBody[nr][nc] != 0
						|| grid[nr][nc] == true) continue;
					else
					{
						tmpMonster[nr][nc][dir] += monster[r][c][d];
						break;
					}
				}
				//이동 불가
				if (i == 8) tmpMonster[r][c][d] += monster[r][c][d];
			}
		}
	}

	copyMonster(monster, tmpMonster);
}


int getMonster(int step)
{
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

void movePackMan(int step)
{
	for (int i = 0; i < 3; i++)
	{
		int nr, nc;

		nr = packMan.r + dr4[position[step][i]];
		nc = packMan.c + dc4[position[step][i]];

		for (int d = 1; d <= 8; d++)
		{
			if (monster[nr][nc][d] == 0) continue;

			monster[nr][nc][d] = 0;
			deadBody[nr][nc] = 3;
		}

		packMan.r = nr;
		packMan.c = nc;
	}
}


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

		// [버그수정] 원본은 1로 시작해서 0마리/1마리만 먹는 경로를 절대 고르지 못했다.
		//            (벽이면 getMonster가 -1을 주므로 -1에서 시작해야 한다)
		step = maxMonster = -1;

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

// [수정] main() -> solution()
//        원본 main의 T 루프 껍데기는 제거했다. 채점기가 케이스마다 한 번씩 부른다.
int solution(int t, int r, int c, std::vector<std::vector<int>> monsters)
{
	pcnt = 0;    // [추가] 재호출 대비 - DFS가 position에 64개를 누적하므로 매번 0에서 시작해야 한다

	DFS(0);

	input(t, r, c, monsters);

	simulate();

	return getAnswer();   // [수정] printf -> return
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
//   빌드      : g++ -O2 -DLOCAL_TEST -o run solution.cpp
//   재호출 검사 : g++ -O2 -DLOCAL_TEST -DREPEAT_TEST -o rep solution.cpp
// 원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력해야 cmp 대조가 성립한다.
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	int m, t, pr, pc;

	scanf("%d %d %d %d", &m, &t, &pr, &pc);   // 원본 scanf 순서 그대로 (원본의 %D 오타는 %d로 정정)

	std::vector<std::vector<int>> monsters(m, std::vector<int>(3));
	for (int i = 0; i < m; i++)
		scanf("%d %d %d", &monsters[i][0], &monsters[i][1], &monsters[i][2]);

	int ans = solution(t, pr, pc, monsters);

#ifdef REPEAT_TEST
	int ans2 = solution(t, pr, pc, monsters);   // 같은 인자로 한 번 더
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);   // 원본 출력 형식 그대로
	return 0;
}
#endif
