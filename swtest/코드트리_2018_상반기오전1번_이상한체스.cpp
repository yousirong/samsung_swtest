// 코드트리 2018 상반기 오전 1번 - 이상한 체스
//
// 문제 요약
// 격자판에 여러 종류의 말(1~5번)이 놓여 있다.
// 각 말은 방향을 하나 정하면 정해진 규칙대로 감시 영역을 만든다.
//
// 칸의 의미
// 0 : 빈 칸
// 6 : 벽
// 1~5 : 말의 종류
//
// 목표
// 모든 말의 방향을 적절히 정해서
// 감시되지 않는 빈 칸(사각지대)의 개수를 최소로 만드는 문제이다.
//
// 핵심 아이디어
// 1. 모든 말에 대해 가능한 방향(0~3)을 DFS로 전부 만들어 본다.
// 2. 방향이 정해지면 simulate()로 실제 감시 영역을 표시한다.
// 3. 남은 빈 칸 개수를 세어 최소값을 갱신한다.
//
// 이 코드는 기본적으로
// "방향 배치 완전탐색(DFS)" + "감시 영역 시뮬레이션"
// 구조이다.

#include <stdio.h>

#define MAX (8 + 5)
#define INF (0x7fff0000)

// 맵의 상태 정의
#define EMPTY (0)   // 빈 칸
#define OTHER (6)   // 벽
#define MARK  (7)   // 감시된 칸 표시용

int T;

int N, M;               // 격자 크기
int MAP[MAX][MAX];      // 원본 맵
int tmpMAP[MAX][MAX];   // 시뮬레이션용 복사 맵

int num_of_cases[MAX];  // 각 말에 대해 선택한 방향 저장

// 말 정보를 저장하는 구조체
struct CHESS
{
	int r;       // 말의 행
	int c;       // 말의 열
	int number;  // 말 종류 (1~5)
};

typedef struct CHESS CHESS;

CHESS chess[8 + 2];     // 말들의 위치 목록
int cidx;               // 말의 개수

int minAnswer;          // 최소 사각지대 개수

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	// 바깥 테두리를 모두 벽(6)으로 초기화
	// 이렇게 하면 checkArea()에서 범위 검사 없이
	// 벽을 만날 때까지 전진하도록 구현할 수 있다.
	for (int r = 0; r <= N + 1; r++)
	{
		for (int c = 0; c <= M + 1; c++)
		{
			MAP[r][c] = OTHER;
		}
	}

	// 실제 격자 입력
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}

	// 말들의 위치와 종류를 따로 저장
	cidx = 0;
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			// 빈 칸(0)과 벽(6)은 말이 아니므로 건너뜀
			if (MAP[r][c] == EMPTY || MAP[r][c] == OTHER)
				continue;

			chess[cidx].r = r;
			chess[cidx].c = c;
			chess[cidx++].number = MAP[r][c];
		}
	}
}

// ---------------------------
// 디버그용 tmpMAP 출력
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
// 배열 복사 함수
// tmpMAP <- MAP
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
// 디버그용: 각 말의 선택 방향 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < cidx; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 하나의 말이 특정 방향으로 감시하는 영역 표시
//
// 시작 위치 (sr, sc)에서 direction 방향으로
// 벽(6)을 만날 때까지 직선으로 전진하며 MARK(7) 표시
//
// 주의:
// - 말이 있는 칸(1~5)도 그냥 지나갈 수 있다.
// - 벽(6)에서만 멈춘다.
// ---------------------------
void checkArea(int sr, int sc, int direction)
{
	int r = sr;
	int c = sc;

	while (1)
	{
		r = r + dr[direction];
		c = c + dc[direction];

		// 벽을 만나면 종료
		if (tmpMAP[r][c] == OTHER)
			return;

		// 그 외 칸은 감시 표시
		tmpMAP[r][c] = MARK;
	}
}

// ---------------------------
// 현재 num_of_cases[]에 저장된 방향대로
// 모든 말의 감시 영역을 tmpMAP에 표시
// ---------------------------
void simulate()
{
	// 원본 맵을 복사해서 시뮬레이션 시작
	copyMap(tmpMAP, MAP);

	// 모든 말에 대해 방향 적용
	for (int i = 0; i < cidx; i++)
	{
		int sr = chess[i].r;
		int sc = chess[i].c;
		int chessNumber = chess[i].number;
		int direction = num_of_cases[i];

		// 1번 말: 한 방향만 감시
		if (chessNumber == 1)
		{
			checkArea(sr, sc, direction);
		}

		// 2번 말: 서로 반대 방향 두 곳 감시
		else if (chessNumber == 2)
		{
			int inverse = direction + 2;
			if (inverse > 3) inverse -= 4;

			checkArea(sr, sc, direction);
			checkArea(sr, sc, inverse);
		}

		// 3번 말: 직각 방향 두 곳 감시
		// 예: 상+우, 우+하, 하+좌, 좌+상
		else if (chessNumber == 3)
		{
			int nextDir = direction + 1;
			if (nextDir == 4) nextDir = 0;

			checkArea(sr, sc, direction);
			checkArea(sr, sc, nextDir);
		}

		// 4번 말: 한 방향만 빼고 3방향 감시
		// 예: direction이 상이면 우/하/좌 감시
		else if (chessNumber == 4)
		{
			for (int i = 0; i < 4; i++)
			{
				if (i == direction) continue;
				checkArea(sr, sc, i);
			}
		}

		// 5번 말: 4방향 모두 감시
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
// tmpMAP에서 남아 있는 빈 칸(사각지대) 개수 세기
//
// tmpMAP[r][c] == 0 이면 아직 감시되지 않은 빈 칸
// ---------------------------
int getArea()
{
	int sum = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			sum += !tmpMAP[r][c];
		}
	}

	return sum;
}

// ---------------------------
// DFS로 각 말의 방향 선택
//
// depth = 현재 몇 번째 말의 방향을 정하는지
//
// 모든 말에 대해 4방향(0~3)을 일단 시도한다.
// 일부 말은 실제로 중복되는 방향이 있더라도
// 이 코드는 단순하게 4가지를 모두 탐색한다.
// ---------------------------
void DFS(int depth)
{
	// 모든 말의 방향을 다 정했으면 시뮬레이션 수행
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

	// 현재 말에 대해 4방향 시도
	for (int i = 0; i < 4; i++)
	{
		num_of_cases[depth] = i;
		DFS(depth + 1);
	}
}

int main()
{
	// 원래 테스트케이스 형식이 있을 수도 있지만
	// 현재 코드는 1회만 수행
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		minAnswer = INF;

		// 모든 방향 배치 탐색
		DFS(0);

		printf("%d\n", minAnswer);
	}

	return 0;
}