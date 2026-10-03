/*
	[코드트리] 2025 상반기 오전 1번 - 민트 초코 우유
	https://www.codetree.ai/training-field/frequent-problems/problems/mint-choco-milk

	■ 문제 요약
	  N x N 교실의 학생마다 신봉하는 음식(T 민트, C 초코, M 우유)과 신앙심 B가 있다.
	  음식은 섞일 수 있어서 TC, TM, CM, TCM 같은 조합도 생긴다. T일 동안 하루는 아래 순서다.

	    1) 아침 : 모든 학생의 신앙심이 1 오른다.
	    2) 점심 : 상하좌우로 이어진 "음식 조합이 완전히 같은" 학생들이 한 그룹이 된다.
	       그룹마다 대표를 뽑는다 (신앙심이 가장 큰 학생 -> 행이 작은 학생 -> 열이 작은 학생).
	       대표는 신앙심이 (그룹 인원 - 1)만큼 오르고, 나머지는 1씩 내려간다.
	    3) 저녁 : 대표들이 차례로 자기 음식을 전파한다.
	       순서는 단일 음식 그룹 -> 이중 조합 -> 삼중 조합이고, 같은 단계에서는 대표 우선순위 순이다.
	       - 이미 전파를 당해 "방어 상태"가 된 대표는 전파하지 않는다.
	       - 방향은 B % 4 (0 위, 1 아래, 2 왼쪽, 3 오른쪽), 간절함 x = B - 1, 자기 신앙심은 1이 된다.
	       - 그 방향으로 한 칸씩 가며, 같은 음식인 학생은 건너뛴다.
	         다른 음식인 학생(신앙심 y)을 만나면 그 학생은 방어 상태가 되고
	            x > y  : 강한 전파. 그 학생은 전파자의 음식이 되고 신앙심 +1, x -= (y + 1)
	            x <= y : 약한 전파. 그 학생의 음식에 전파자 음식이 합쳐지고 신앙심 += x, 전파 끝
	       - 격자 밖으로 나가거나 x가 0이 되면 끝난다.

	  하루가 끝날 때마다 음식 조합별 신앙심 합을 TCM, TC, TM, CM, M, C, T 순서로 출력한다.

	■ 음식을 숫자 하나로 담는 방법
	  T = 1, C = 10, M = 100 으로 두고 조합은 더한 값으로 본다.
	      TC = 11, TM = 101, CM = 110, TCM = 111
	  십진수 각 자리가 "그 음식을 믿는가"를 뜻하므로, 음식 종류 수도 자리 개수로 바로 알 수 있다.

	  섞을 때는 mixFood에서 비트 OR(|)를 쓴다. 십진 표기인데 OR가 맞는 이유는
	  1 = 0b1, 10 = 0b1010, 100 = 0b1100100 의 켜진 비트가 하나도 겹치지 않기 때문이다.
	  그래서 OR가 곧 덧셈이고, 이미 들어 있는 음식은 두 번 더해지지 않는다.
	  (7가지 조합끼리 49가지 OR를 모두 확인해 봤고 전부 맞다)

	■ 풀이 방침
	  - 점심의 그룹은 BFS로 찾는다. "음식 값이 완전히 같은 이웃"만 따라간다.
	    한 그룹을 다 모은 뒤 isPriority로 대표를 고르고 신앙심을 조정한다.
	  - 저녁은 대표만 골라 그룹(1, 2, 3)별로 나눈 뒤 각 그룹 안에서 정렬한다.
	    정렬 기준이 대표 선정 기준과 같으므로 isPriority를 그대로 다시 쓴다.
	  - 방어 상태는 defense 값으로 표시하고 저녁이 끝나면 모두 지운다.

	■ 주의할 점
	  - 대표 정렬은 저녁이 "시작될 때"의 값으로 한 번만 한다.
	    전파 도중 신앙심이 바뀌어도 순서는 다시 정하지 않는다.
	    대신 전파할 때는 student[][]에서 지금 값을 다시 읽어 쓴다(spreader).
	  - 전파를 당해 음식이 바뀐 대표도, 방어 상태라서 그날은 전파하지 않는다.
	  - 전역 이름 index 는 리눅스 <strings.h>의 index() 함수와 겹칠 수 있다.
	    stdio.h만 쓰는 지금은 괜찮지만 다른 헤더를 붙일 때는 이름을 바꾸는 편이 안전하다.
*/
#include <stdio.h>

#define MAX (50 + 5)

#define TMINT_CHOKO_MILK (111)
// T = 1, C = 10, M = 100 을 더한 값으로 조합을 나타낸다.
#define TMINT_CHOKO (1 + 10)
#define TMINT_MILK (1 + 100)
#define CHOKO_MILK (100 + 10)
#define MILK (100)
#define CHOKO (10)
#define TMINT (1)

#define UP (0)
#define DOWN (1)
#define LEFT (2)
#define RIGHT (3)

int TestCase;
int N, T;
char F[MAX][MAX];
int B[MAX][MAX];

struct STUDENT
{
	int food; // 신봉하는 음식
	int believe; // 신앙심
	bool isLeader; // 대표
	int defense; // 방어 여부
	int row; // 대표자 선정을 위한 값
	int col;
};

STUDENT student[MAX][MAX];
STUDENT candidates[MAX * MAX];
int ccnt;

STUDENT group[3 + 1][MAX * MAX];
int index[3 + 1];

struct RC
{
	int r;
	int c;
};

RC queue[MAX * MAX];
bool visit[MAX][MAX];

// 위, 아래, 왼쪽, 오른쪽
int dr[4] = { -1, 1, 0, 0 };
int dc[4] = { 0, 0, -1, 1 };

void input()
{
	scanf("%d %d", &N, &T);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf(" %c", &F[r][c]);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &B[r][c]);

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			student[r][c].believe = B[r][c];
			student[r][c].isLeader = false; // 초기화
			student[r][c].defense = 0; // 초기화
			student[r][c].row = r;
			student[r][c].col = c;

			if (F[r][c] == 'T') student[r][c].food = TMINT;
			else if (F[r][c] == 'C') student[r][c].food = CHOKO;
			else if (F[r][c] == 'M') student[r][c].food = MILK;
		}
	}
}

void printBelieve() // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			printf("%d ", student[r][c].believe);
		}
		putchar('\n');
	}
	putchar('\n');
}

void printFood() // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			printf("%3d ", student[r][c].food);
		}
		putchar('\n');
	}
	putchar('\n');
}

void printGroup() // for debug
{
	for (int g = 1; g <= 3; g++)
	{
		int gIndex = index[g];
		printf("group %d ", g);

		for (int i = 0; i < gIndex; i++)
			printf("(%d, %d) ", group[g][i].row, group[g][i].col);

		putchar('\n');
	}
}

void morning()
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			student[r][c].believe++;
}

// a가 우선순위가 더 높으면 true
bool isPriority(STUDENT a, STUDENT b)
{
	if (a.believe != b.believe) return a.believe > b.believe;
	// 신앙심이 크면 우선, 같으면 행 -> 열이 작은 쪽
	if (a.row != b.row) return a.row < b.row;

	return a.col < b.col;
}

void BFS(int r, int c)
{
	int rp, wp;

	rp = wp = 0;

	queue[wp].r = r;
	queue[wp++].c = c;

	visit[r][c] = true;

	ccnt = 0;
	candidates[ccnt++] = student[r][c];

	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			if (nr < 1 || nc < 1 || nr > N || nc > N) continue;
			if (visit[nr][nc] == true) continue;

			// 인접한 학생들과 신봉 음식이 완전히 같은 경우에만 그룹을 형성
			if (student[out.r][out.c].food != student[nr][nc].food) continue;
			// (조합까지 완전히 같아야 한다. TC와 T는 다른 그룹이다)

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = true;

			candidates[ccnt++] = student[nr][nc];
		}
	}

	STUDENT leader = { 0 };
	for (int i = 0; i < ccnt; i++)
		if (isPriority(candidates[i], leader) == true)
			leader = candidates[i];

	student[leader.row][leader.col].isLeader = true;
	student[leader.row][leader.col].believe += (ccnt - 1);
	// 대표는 (인원 - 1)만큼 오르고, 아래에서 나머지는 1씩 내린다.

	for (int i = 0; i < ccnt; i++)
	{
		STUDENT c = candidates[i];

		if (c.row == leader.row && c.col == leader.col) continue;

		student[c.row][c.col].believe--;
	}
}

void lunch()
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			visit[r][c] = student[r][c].isLeader = false;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (visit[r][c] == true) continue;

			BFS(r, c);
		}
	}
}

int getGroupNumber(int food)
{
	if (food == TMINT_CHOKO_MILK) return 3;
	// 단일 음식 1단계, 두 가지 조합 2단계, 세 가지 조합 3단계
	if (food == TMINT || food == CHOKO || food == MILK) return 1;

	return 2;
}

int mixFood(int sFood, int nFood)
{
	return sFood | nFood;
	// 1, 10, 100의 비트가 겹치지 않아 OR가 곧 "음식 합치기"다.
}

void dinner()
{
	for (int g = 1; g <= 3; g++) index[g] = 0;

	// 그룹 별 분리
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			STUDENT s = student[r][c];

			if (s.isLeader == false) continue;

			int groupNumber = getGroupNumber(s.food);

			group[groupNumber][index[groupNumber]++] = s;
		}
	}

	// 그룹 내 정렬
	for (int g = 1; g <= 3; g++)
	{
		int gIndex = index[g];
		for (int i = 0; i < gIndex - 1; i++)
		{
			for (int k = i + 1; k < gIndex; k++)
			{
				STUDENT a = group[g][i];
				STUDENT b = group[g][k];

				if (isPriority(a, b) == false)
				{
					group[g][i] = b;
					group[g][k] = a;
				}
			}
		}
	}

	// printGroup();

	// 전파 시작
	for (int g = 1; g <= 3; g++)
	{
		int gIndex = index[g];
		for (int i = 0; i < gIndex; i++)
		{
			int gr, gc;

			gr = group[g][i].row;
			gc = group[g][i].col;

			STUDENT spreader = student[gr][gc];

			// 방어 상태에서는 대표자가 되어도 전파를 하지 않음.
			if (spreader.defense != 0) continue;

			int sr, sc;

			sr = spreader.row;
			sc = spreader.col;

			// 원본의 신앙심을 1로 변경
			student[sr][sc].believe = 1;

			int x = spreader.believe - 1; // 간절함
			int dir = spreader.believe % 4;

			while (1)
			{
				int nr, nc;

				nr = sr + dr[dir];
				nc = sc + dc[dir];

				// 격자 밖으로 나가거나 간절함이 0이 되면 전파 종료
				if (nr < 1 || nc < 1 || nr > N || nc > N || x == 0) break;

				// 전파 대상
				STUDENT next = student[nr][nc];

				// 전파 대상이 전파자와 신봉 음식이 완전히 같은 경우
				if (next.food == spreader.food)
				{
					// 전파를 하지 않고 바로 다음으로 진행
					sr = nr;
					sc = nc;

					continue;
				}

				// 전파 대상이 전파자와 신봉 음식이 다른 경우, 전파 시도
				int y = next.believe; // 신앙심

				// 대표자에게 전파가 시도된 학생은 방어 상태
				student[next.row][next.col].defense = 1;
				// 전파를 당한 학생은 오늘 대표여도 전파하지 못한다.

				if (x > y) // 강한 전파 성공
				{
					x -= (y + 1);
					if (x < 0) x = 0;

					student[next.row][next.col].believe++;
					student[next.row][next.col].food = spreader.food;
				}
				else // x <= y, 약한 전파 성공
				{
					student[next.row][next.col].believe += x;
					student[next.row][next.col].food = mixFood(spreader.food, next.food);

					x = 0;

					break;
				}

				sr = nr;
				sc = nc;
			}
		}
	}

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			student[r][c].defense = 0;
}

void printAnswer()
{
	int sum[TMINT_CHOKO_MILK + 1] = { 0 };

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			sum[student[r][c].food] += student[r][c].believe;

	int outputIndex[7]
		= { TMINT_CHOKO_MILK, TMINT_CHOKO, TMINT_MILK, CHOKO_MILK, MILK, CHOKO, TMINT };

	for (int i = 0; i < 7; i++)
		printf("%d ", sum[outputIndex[i]]);
	putchar('\n');
}

void simulate()
{
	for (int t = 0; t < T; t++)
	{
		morning();
		lunch();
		dinner();

		printAnswer();
	}
}

int main()
{
	// scanf("%d", &TestCase);
	TestCase = 1;
	for (int tc = 1; tc <= TestCase; tc++)
	{
		input();

		simulate();
	}

	return 0;
}
