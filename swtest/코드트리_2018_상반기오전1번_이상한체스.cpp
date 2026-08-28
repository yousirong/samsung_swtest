/*
	[코드트리] 2018 상반기 오전 1번 - 이상한 체스
	https://www.codetree.ai/training-field/frequent-problems/problems/odd-chess

	■ 문제 요약
	  N x M 격자에 말이 놓여 있다.

	    0    : 빈 칸
	    6    : 벽
	    1~5  : 말 (종류에 따라 감시하는 방향 모양이 다르다)

	  각 말은 놓인 방향을 90도 단위로 자유롭게 정할 수 있고,
	  정해진 모양대로 벽에 막힐 때까지 직선으로 감시한다.
	  모든 말의 방향을 잘 정해서 감시되지 않는 빈 칸(사각지대)의 개수를 최소로 만든다.

	  (백준 15683 "감시"와 같은 문제다)

	■ 말의 종류
	    1번 : 한 방향
	    2번 : 서로 반대인 두 방향       (상+하 또는 좌+우)
	    3번 : 직각을 이루는 두 방향     (상+우, 우+하, 하+좌, 좌+상)
	    4번 : 한 방향만 빼고 세 방향
	    5번 : 네 방향 모두

	■ 풀이 방침 : 방향 배치 완전탐색 + 감시 영역 시뮬레이션
	  말이 최대 8개이고 각 말의 방향이 4가지이므로 조합은 최대 4^8 = 65,536가지다.
	  전부 만들어 보고 그때마다 감시 영역을 칠한 뒤 남은 빈 칸을 세면 된다.

	    1) DFS로 말마다 방향 0 ~ 3을 정한다
	    2) 원본을 복사한 tmpMAP에 모든 말의 감시 영역을 MARK로 칠한다
	    3) 남은 0의 개수를 세어 최솟값을 갱신한다

	  5번 말처럼 방향을 돌려도 결과가 같은 말은 4가지를 다 볼 필요가 없지만
	  (2번은 2가지, 5번은 1가지면 충분하다), 경우의 수가 작아서 그냥 4가지를 다 돈다.

	■ 감시 영역 칠하기 : checkArea()
	  시작 칸에서 그 방향으로 한 칸씩 나아가며 벽(6)을 만날 때까지 MARK(7)를 칠한다.

	    - 다른 말이 있는 칸(1~5)은 감시를 막지 않는다. 그냥 통과한다.
	      (그래서 벽만 검사하고, 칠할 때는 말 위에 MARK를 덮어써도 무방하다)
	    - 격자 바깥을 전부 벽으로 채워 뒀기 때문에 좌표 범위 검사가 필요 없다.

	■ 마지막에 남은 0 세기
	  말이 있던 칸은 1~5, 감시된 칸은 7, 벽은 6이므로
	  값이 0인 칸은 "감시되지 않은 빈 칸"뿐이다. 그래서 0의 개수가 곧 사각지대 수다.
	  (`sum += !tmpMAP[r][c]` 는 값이 0일 때만 1을 더하는 관용 표현)

	■ 사소한 참고
	  simulate() 안에서 4번/5번 말을 처리하는 for문이 바깥 루프와 같은 이름 i를 쓴다.
	  안쪽 블록에서만 유효한 별개의 변수라 동작에는 문제가 없지만, 읽을 때 헷갈리기 쉽다.
*/

#include <stdio.h>

#define MAX (8 + 5)
#define INF (0x7fff0000)

// 칸의 상태
#define EMPTY (0)   // 빈 칸
#define OTHER (6)   // 벽
#define MARK  (7)   // 감시된 칸 표시

int T;

int N, M;               // 격자 크기
int MAP[MAX][MAX];      // 원본 (건드리지 않는다)
int tmpMAP[MAX][MAX];   // 조합마다 새로 복사해 칠하는 작업용 격자

int num_of_cases[MAX];  // 말마다 고른 방향

// 말 하나의 정보
struct CHESS
{
	int r;       // 행
	int c;       // 열
	int number;  // 종류 (1 ~ 5)
};

typedef struct CHESS CHESS;

CHESS chess[8 + 2];     // 말 목록
int cidx;               // 말 개수

int minAnswer;          // 사각지대의 최솟값

// 방향 배열: 상(0), 우(1), 하(2), 좌(3) - 시계 방향 순서
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	// 격자 바깥을 전부 벽으로 채워 둔다.
	// 그래야 checkArea가 "벽을 만날 때까지"라는 조건 하나로 안전하게 멈춘다.
	for (int r = 0; r <= N + 1; r++)
	{
		for (int c = 0; c <= M + 1; c++)
		{
			MAP[r][c] = OTHER;
		}
	}

	// 실제 격자
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}

	// 말의 위치와 종류를 따로 목록으로 모아 둔다.
	// 이후 DFS는 이 목록의 순서대로 방향을 정하면 된다.
	cidx = 0;
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			// 빈 칸과 벽은 말이 아니다
			if (MAP[r][c] == EMPTY || MAP[r][c] == OTHER)
				continue;

			chess[cidx].r = r;
			chess[cidx].c = c;
			chess[cidx++].number = MAP[r][c];
		}
	}
}

// ---------------------------
// 디버그용: 테두리까지 포함해 작업 격자 출력
// ---------------------------
void printMap()
{
	for (int r = 0; r <= N + 1; r++)
	{
		for (int c = 0; c <= M + 1; c++)
			printf("%d ", tmpMAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// original -> copy 로 격자 복사 (테두리 포함)
// ---------------------------
void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int i = 0; i <= N + 1; i++)
	{
		for (int k = 0; k <= M + 1; k++)
		{
			copy[i][k] = original[i][k];
		}
	}
}

// ---------------------------
// 디버그용: 말마다 고른 방향 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < cidx; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// (sr, sc)에서 direction 방향으로 벽을 만날 때까지 감시 표시
//
// 다른 말이 있는 칸은 막지 않으므로 벽(6)만 검사한다.
// ---------------------------
void checkArea(int sr, int sc, int direction)
{
	int r = sr;
	int c = sc;

	while (1)
	{
		r = r + dr[direction];
		c = c + dc[direction];

		// 벽에 닿으면 멈춘다 (격자 바깥도 벽이라 여기서 걸린다)
		if (tmpMAP[r][c] == OTHER)
			return;

		tmpMAP[r][c] = MARK;
	}
}

// ---------------------------
// 현재 방향 조합대로 모든 말의 감시 영역을 칠한다
// ---------------------------
void simulate()
{
	// 매번 원본에서 새로 시작
	copyMap(tmpMAP, MAP);

	for (int i = 0; i < cidx; i++)
	{
		int sr = chess[i].r;
		int sc = chess[i].c;
		int chessNumber = chess[i].number;
		int direction = num_of_cases[i];

		// 1번 : 고른 방향 하나
		if (chessNumber == 1)
		{
			checkArea(sr, sc, direction);
		}

		// 2번 : 고른 방향과 그 반대 방향
		//       방향 번호가 시계 순서라 +2가 정반대다 (넘치면 -4로 되돌림)
		else if (chessNumber == 2)
		{
			int inverse = direction + 2;
			if (inverse > 3) inverse -= 4;

			checkArea(sr, sc, direction);
			checkArea(sr, sc, inverse);
		}

		// 3번 : 고른 방향과 그 시계 방향 다음 (직각 두 방향)
		else if (chessNumber == 3)
		{
			int nextDir = direction + 1;
			if (nextDir == 4) nextDir = 0;

			checkArea(sr, sc, direction);
			checkArea(sr, sc, nextDir);
		}

		// 4번 : 고른 방향 하나만 빼고 나머지 세 방향
		else if (chessNumber == 4)
		{
			for (int i = 0; i < 4; i++)
			{
				if (i == direction) continue;
				checkArea(sr, sc, i);
			}
		}

		// 5번 : 네 방향 모두 (방향 선택이 결과에 영향을 주지 않는다)
		else if (chessNumber == 5)
		{
			for (int i = 0; i < 4; i++)
			{
				checkArea(sr, sc, i);
			}
		}
	}
}

// ---------------------------
// 감시되지 않고 남은 빈 칸(사각지대) 개수
// ---------------------------
int getArea()
{
	int sum = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			// 말(1~5), 벽(6), 감시됨(7)은 모두 0이 아니다.
			// 값이 0인 칸만 사각지대다.
			sum += !tmpMAP[r][c];
		}
	}

	return sum;
}

// ---------------------------
// DFS : 말마다 방향을 하나씩 정한다
//
//   depth : 지금 방향을 정하려는 말의 인덱스
// ---------------------------
void DFS(int depth)
{
	// 모든 말의 방향이 정해졌으면 실제로 칠해 본다
	if (depth == cidx)
	{
		// printCases();

		simulate();

		// printMap();

		int tmp = getArea();
		if (tmp < minAnswer)
			minAnswer = tmp;

		return;
	}

	// 말 하나당 네 방향 (중복 허용 = 중복순열)
	for (int i = 0; i < 4; i++)
	{
		num_of_cases[depth] = i;
		DFS(depth + 1);
	}
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		minAnswer = INF;

		DFS(0);

		printf("%d\n", minAnswer);
	}

	return 0;
}
