// 코드트리 2016 하반기 2번 - 2048 게임
// https://www.codetree.ai/training-field/frequent-problems/problems/2048-game
// 문제 요약
// N x N 보드에서 2048 게임을 최대 5번 이동시킬 수 있을 때,
// 만들 수 있는 가장 큰 블록 값을 구하는 문제이다.
//
// 이동 방향
// - 위
// - 오른쪽
// - 아래
// - 왼쪽
//
// 핵심 아이디어
// 1. 5번의 이동 방향 수열을 DFS로 전부 만들어 본다.
//    각 이동은 4가지 방향 중 하나이므로 총 4^5 = 1024가지.
// 2. 각 방향 수열마다 실제로 2048 보드를 시뮬레이션한다.
// 3. 시뮬레이션 결과에서 가장 큰 블록 값을 구해 최댓값을 갱신한다.
//
// 이 코드의 특징
// - DFS로 방향 수열 생성
// - simulate()에서 원본 맵을 복사한 뒤 이동 5번 수행
// - 각 move 함수는
//   "압축 -> 같은 수 합치기 -> 다시 정리"
//   과정을 한 번에 처리한다.

#include <stdio.h>

#define MAX (20 + 5)

// 방향 번호 정의
#define UP (0)
#define RIGHT (1)
#define DOWN (2)
#define LEFT (3)

int T;

int N;
int oMAP[MAX][MAX];     // original Map: 원본 보드
int ansMAP[MAX][MAX];   // 시뮬레이션 중 실제로 변형할 보드

int num_of_cases[5 + 5]; // DFS로 만든 5번 이동 방향 수열

int maxAnswer;          // 만들 수 있는 가장 큰 블록 값

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d", &N);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &oMAP[r][c]);
}

// ---------------------------
// 디버그용 맵 출력
// ---------------------------
void printMap(int map[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 배열 복사 함수
// copy <- original
// ---------------------------
void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			copy[r][c] = original[r][c];
}

// ---------------------------
// 디버그용 방향 수열 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < 5; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// --------------------------------------------------
// 위로 이동
// --------------------------------------------------
//
// 한 열(column)씩 처리한다.
//
// 처리 순서
// 1. 0이 아닌 값들을 위쪽으로 압축
// 2. 위에서 아래로 보며 인접한 같은 값을 합침
// 3. 합쳐진 뒤 빈 칸을 당겨 정리
//
// 예)
// [2, 0, 2, 4] -> 압축 [2, 2, 4, 0]
//                -> 합치기 [4, 4, 0, 0]
void moveUp()
{
	int tmpMAP[MAX][MAX] = { 0 };

	// 각 열을 독립적으로 처리
	for (int c = 1; c <= N; c++)
	{
		int index = 1;  // 위에서부터 채울 위치

		// 1단계: 0이 아닌 블록을 위로 압축
		for (int r = 1; r <= N; r++)
		{
			if (ansMAP[r][c] == 0)
				continue;

			tmpMAP[index++][c] = ansMAP[r][c];
		}

		// 2단계: 위에서 아래로 보며 같은 값 합치기
		for (int r = 1; r <= N - 1; r++)
		{
			// 다음 블록과 값이 다르면 합칠 수 없음
			if (tmpMAP[r][c] != tmpMAP[r + 1][c])
				continue;

			// 0이면 더 볼 필요 없음
			if (tmpMAP[r][c] == 0)
				break;

			// 현재 블록을 2배로 만듦
			tmpMAP[r][c] *= 2;

			// 합쳐진 뒤 뒤쪽 블록들을 한 칸씩 앞으로 당김
			for (int tr = r + 1; tr <= N - 1; tr++)
				tmpMAP[tr][c] = tmpMAP[tr + 1][c];

			// 마지막 칸은 0 처리
			tmpMAP[N][c] = 0;
		}
	}

	// 결과를 ansMAP에 반영
	copyMap(ansMAP, tmpMAP);
}

// --------------------------------------------------
// 오른쪽으로 이동
// --------------------------------------------------
//
// 한 행(row)씩 처리한다.
// 오른쪽 끝부터 압축하고,
// 오른쪽에서 왼쪽 방향으로 합친다.
void moveRight()
{
	int tmpMAP[MAX][MAX] = { 0 };

	// 각 행을 독립적으로 처리
	for (int r = 1; r <= N; r++)
	{
		int index = N; // 오른쪽 끝부터 채울 위치

		// 1단계: 0이 아닌 블록을 오른쪽으로 압축
		for (int c = N; c >= 1; c--)
		{
			if (ansMAP[r][c] == 0)
				continue;

			tmpMAP[r][index--] = ansMAP[r][c];
		}

		// 2단계: 오른쪽에서 왼쪽으로 보며 같은 값 합치기
		for (int c = N; c >= 2; c--)
		{
			if (tmpMAP[r][c] != tmpMAP[r][c - 1])
				continue;

			if (tmpMAP[r][c] == 0)
				break;

			// 기준 블록 2배
			tmpMAP[r][c] *= 2;

			// 왼쪽에 남은 블록들을 한 칸씩 당김
			for (int tc = c - 1; tc >= 2; tc--)
				tmpMAP[r][tc] = tmpMAP[r][tc - 1];

			// 맨 왼쪽은 0 처리
			tmpMAP[r][1] = 0;
		}
	}

	copyMap(ansMAP, tmpMAP);
}

// --------------------------------------------------
// 아래로 이동
// --------------------------------------------------
//
// 한 열씩 처리한다.
// 아래쪽 끝부터 압축하고,
// 아래에서 위 방향으로 합친다.
void moveDown()
{
	int tmpMAP[MAX][MAX] = { 0 };

	for (int c = 1; c <= N; c++)
	{
		int index = N; // 아래쪽 끝부터 채울 위치

		// 1단계: 0이 아닌 블록을 아래로 압축
		for (int r = N; r >= 1; r--)
		{
			if (ansMAP[r][c] == 0)
				continue;

			tmpMAP[index--][c] = ansMAP[r][c];
		}

		// 2단계: 아래에서 위로 보며 같은 값 합치기
		for (int r = N; r >= 2; r--)
		{
			if (tmpMAP[r][c] != tmpMAP[r - 1][c])
				continue;

			if (tmpMAP[r][c] == 0)
				break;

			// 기준 블록 2배
			tmpMAP[r][c] *= 2;

			// 위쪽 블록들을 한 칸씩 아래로 당김
			for (int tr = r - 1; tr >= 2; tr--)
				tmpMAP[tr][c] = tmpMAP[tr - 1][c];

			// 맨 위는 0 처리
			tmpMAP[1][c] = 0;
		}
	}

	copyMap(ansMAP, tmpMAP);
}

// --------------------------------------------------
// 왼쪽으로 이동
// --------------------------------------------------
//
// 한 행씩 처리한다.
// 왼쪽으로 압축하고,
// 왼쪽에서 오른쪽 방향으로 합친다.
void moveLeft()
{
	int tmpMAP[MAX][MAX] = { 0 };

	for (int r = 1; r <= N; r++)
	{
		int index = 1; // 왼쪽부터 채울 위치

		// 1단계: 0이 아닌 블록을 왼쪽으로 압축
		for (int c = 1; c <= N; c++)
		{
			if (ansMAP[r][c] == 0)
				continue;

			tmpMAP[r][index++] = ansMAP[r][c];
		}

		// 2단계: 왼쪽에서 오른쪽으로 보며 같은 값 합치기
		for (int c = 1; c <= N - 1; c++)
		{
			if (tmpMAP[r][c] != tmpMAP[r][c + 1])
				continue;

			if (tmpMAP[r][c] == 0)
				break;

			// 기준 블록 2배
			tmpMAP[r][c] *= 2;

			// 오른쪽 블록들을 한 칸씩 앞으로 당김
			for (int tc = c + 1; tc <= N - 1; tc++)
				tmpMAP[r][tc] = tmpMAP[r][tc + 1];

			// 마지막 칸은 0 처리
			tmpMAP[r][N] = 0;
		}
	}

	copyMap(ansMAP, tmpMAP);
}

// --------------------------------------------------
// 현재 num_of_cases[] 방향 수열대로 실제 보드를 시뮬레이션
// --------------------------------------------------
void simulate()
{
	// 시뮬레이션 시작 전 원본 보드 복사
	copyMap(ansMAP, oMAP);

	// 5번 이동 수행
	for (int i = 0; i < 5; i++)
	{
		int direction = num_of_cases[i];

		if (direction == UP)
			moveUp();
		else if (direction == RIGHT)
			moveRight();
		else if (direction == DOWN)
			moveDown();
		else if (direction == LEFT)
			moveLeft();
	}
}

// --------------------------------------------------
// 현재 ansMAP에서 가장 큰 블록 값을 찾음
// --------------------------------------------------
int findMax()
{
	int maxValue = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (maxValue < ansMAP[r][c])
				maxValue = ansMAP[r][c];
		}
	}

	return maxValue;
}

// --------------------------------------------------
// DFS로 길이 5의 방향 수열 생성
//
// depth = 현재 몇 번째 이동을 정하고 있는지
// --------------------------------------------------
void DFS(int depth)
{
	// 5번 이동 방향이 모두 정해졌으면 시뮬레이션
	if (depth == 5)
	{
		// printCases();

		simulate();

		int tmp = findMax();
		if (maxAnswer < tmp)
			maxAnswer = tmp;

		return;
	}

	// 각 이동은 4방향 중 하나
	for (int i = 0; i < 4; i++)
	{
		num_of_cases[depth] = i;
		DFS(depth + 1);
	}
}

int main()
{
	// 원래 테스트 케이스 입력이 있을 수도 있지만
	// 현재 코드는 1회만 수행
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		maxAnswer = 0;

		// 모든 방향 수열 탐색
		DFS(0);

		printf("%d\n", maxAnswer);
	}

	return 0;
}