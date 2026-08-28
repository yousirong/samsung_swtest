/*
	[코드트리] 2016 하반기 2번 - 2048 게임
	https://www.codetree.ai/training-field/frequent-problems/problems/2048-game

	■ 문제 요약
	  N x N 보드에 2의 거듭제곱 블록들이 놓여 있다.
	  상/하/좌/우 중 한 방향으로 보드를 기울이면 모든 블록이 그 방향 끝까지 밀려가고,
	  진행 방향으로 맞닿은 같은 값 두 개는 합쳐져 두 배가 된다.
	  최대 5번 움직였을 때 만들 수 있는 가장 큰 블록의 값을 구한다.

	■ 풀이 방침 : 방향 수열 완전탐색 + 시뮬레이션
	  이동 횟수가 5번뿐이라 방향 조합이 4^5 = 1024가지밖에 안 된다.
	  좋은 순서를 추론할 필요 없이 전부 해 보면 된다.

	    1) DFS로 길이 5짜리 방향 수열을 만든다
	    2) 원본 보드를 복사한 뒤 그 순서대로 5번 이동시킨다
	    3) 결과 보드에서 가장 큰 값을 찾아 최댓값을 갱신한다

	  원본은 oMAP에 그대로 두고 ansMAP에서만 굴리는 것이 중요하다.
	  그래야 다음 방향 수열도 같은 초기 상태에서 다시 시작할 수 있다.

	■ 한 번의 이동 처리 : 압축 -> 합치기
	  네 방향 함수가 모두 같은 골격이고, 훑는 방향만 다르다.

	    1단계 (압축) : 0이 아닌 블록만 골라 진행 방향 끝부터 차곡차곡 채운다.
	                   중간의 빈칸이 전부 사라진다.
	    2단계 (합치기): 진행 방향 쪽 끝에서부터 이웃한 두 칸을 비교해,
	                   값이 같으면 앞쪽 칸을 2배로 만들고 뒤쪽 블록들을 한 칸씩 당긴다.

	  압축을 먼저 해 두기 때문에 "사이에 빈칸이 있는 같은 값"도 자연히 맞닿게 되어
	  따로 처리할 필요가 없다.

	■ 합치기에서 가장 헷갈리는 부분 : 한 번 합쳐진 블록은 다시 합쳐지지 않는다
	  이 코드는 r번 칸을 합친 뒤 곧바로 r+1로 넘어간다.
	  이미 2배가 된 r번 칸은 다시 비교 대상이 되지 않으므로 연쇄 합체가 생기지 않는다.

	    [2, 2, 2, 2] 를 위로 : r=1에서 합쳐 [4, 2, 2, 0] -> r=2에서 합쳐 [4, 4, 0, 0]
	                           (남은 4와 4는 서로 합쳐지지 않는다)
	    [2, 2, 4, 4] 를 위로 : [4, 4, 4, 0] -> [4, 8, 0, 0]

	  두 예 모두 실제 2048 규칙과 일치한다.

	■ 참고
	  직전과 같은 방향을 연달아 고르는 것은 상태가 변하지 않아 의미가 없으므로
	  가지치기할 수 있지만, 1024가지는 그냥 다 돌려도 충분히 빠르다.
*/

#include <stdio.h>

#define MAX (20 + 5)

// 방향 번호
#define UP (0)
#define RIGHT (1)
#define DOWN (2)
#define LEFT (3)

int T;

int N;
int oMAP[MAX][MAX];     // original map : 입력 원본 (절대 건드리지 않는다)
int ansMAP[MAX][MAX];   // 시뮬레이션용 작업 보드

int num_of_cases[5 + 5]; // DFS로 만든 이동 방향 수열 (길이 5)

int maxAnswer;          // 만들 수 있는 가장 큰 블록 값

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d", &N);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &oMAP[r][c]);
}

// ---------------------------
// 디버그용 보드 출력
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
// original -> copy 로 보드 복사
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
// 위로 이동 : 열 단위로 처리한다
//
//   [2, 0, 2, 4]  --압축-->  [2, 2, 4, 0]  --합치기-->  [4, 4, 0, 0]
// --------------------------------------------------
void moveUp()
{
	// 결과를 새 배열에 만든다. 0으로 초기화되어 있으므로
	// 채우지 않은 칸은 자동으로 빈칸이 된다.
	int tmpMAP[MAX][MAX] = { 0 };

	// 열끼리는 서로 영향을 주지 않으므로 한 열씩 독립적으로 처리
	for (int c = 1; c <= N; c++)
	{
		int index = 1;  // 위에서부터 채워 넣을 위치

		// 1단계 : 0이 아닌 블록만 위쪽으로 몰아 넣는다 (빈칸 제거)
		for (int r = 1; r <= N; r++)
		{
			if (ansMAP[r][c] == 0)
				continue;

			tmpMAP[index++][c] = ansMAP[r][c];
		}

		// 2단계 : 위에서부터 이웃한 두 칸을 비교해 같으면 합친다
		for (int r = 1; r <= N - 1; r++)
		{
			// 값이 다르면 합칠 수 없다
			if (tmpMAP[r][c] != tmpMAP[r + 1][c])
				continue;

			// 압축을 마친 상태라 0을 만났다는 건 이 아래로는 전부 빈칸이라는 뜻
			if (tmpMAP[r][c] == 0)
				break;

			// 앞쪽 칸을 2배로
			tmpMAP[r][c] *= 2;

			// 합쳐지면서 한 칸이 비므로 뒤쪽 블록들을 한 칸씩 당긴다
			for (int tr = r + 1; tr <= N - 1; tr++)
				tmpMAP[tr][c] = tmpMAP[tr + 1][c];

			tmpMAP[N][c] = 0;

			// 여기서 루프가 r+1로 넘어가므로, 방금 2배가 된 r번 칸은
			// 다시 비교되지 않는다 = 연쇄 합체가 일어나지 않는다
		}
	}

	copyMap(ansMAP, tmpMAP);
}

// --------------------------------------------------
// 오른쪽으로 이동 : 행 단위로, 오른쪽 끝부터 채우고 오른쪽에서 왼쪽으로 합친다
// --------------------------------------------------
void moveRight()
{
	int tmpMAP[MAX][MAX] = { 0 };

	for (int r = 1; r <= N; r++)
	{
		int index = N; // 오른쪽 끝부터 채워 넣을 위치

		// 1단계 : 오른쪽부터 훑으며 0이 아닌 블록을 오른쪽으로 몰아 넣는다
		for (int c = N; c >= 1; c--)
		{
			if (ansMAP[r][c] == 0)
				continue;

			tmpMAP[r][index--] = ansMAP[r][c];
		}

		// 2단계 : 오른쪽 끝에서부터 왼쪽 이웃과 비교해 합친다
		for (int c = N; c >= 2; c--)
		{
			if (tmpMAP[r][c] != tmpMAP[r][c - 1])
				continue;

			if (tmpMAP[r][c] == 0)
				break;

			tmpMAP[r][c] *= 2;

			// 왼쪽에 남은 블록들을 오른쪽으로 한 칸씩 당긴다
			for (int tc = c - 1; tc >= 2; tc--)
				tmpMAP[r][tc] = tmpMAP[r][tc - 1];

			tmpMAP[r][1] = 0;
		}
	}

	copyMap(ansMAP, tmpMAP);
}

// --------------------------------------------------
// 아래로 이동 : 열 단위로, 아래 끝부터 채우고 아래에서 위로 합친다
// --------------------------------------------------
void moveDown()
{
	int tmpMAP[MAX][MAX] = { 0 };

	for (int c = 1; c <= N; c++)
	{
		int index = N; // 아래 끝부터 채워 넣을 위치

		// 1단계 : 아래부터 훑으며 0이 아닌 블록을 아래로 몰아 넣는다
		for (int r = N; r >= 1; r--)
		{
			if (ansMAP[r][c] == 0)
				continue;

			tmpMAP[index--][c] = ansMAP[r][c];
		}

		// 2단계 : 아래 끝에서부터 위쪽 이웃과 비교해 합친다
		for (int r = N; r >= 2; r--)
		{
			if (tmpMAP[r][c] != tmpMAP[r - 1][c])
				continue;

			if (tmpMAP[r][c] == 0)
				break;

			tmpMAP[r][c] *= 2;

			// 위쪽에 남은 블록들을 아래로 한 칸씩 당긴다
			for (int tr = r - 1; tr >= 2; tr--)
				tmpMAP[tr][c] = tmpMAP[tr - 1][c];

			tmpMAP[1][c] = 0;
		}
	}

	copyMap(ansMAP, tmpMAP);
}

// --------------------------------------------------
// 왼쪽으로 이동 : 행 단위로, 왼쪽부터 채우고 왼쪽에서 오른쪽으로 합친다
// --------------------------------------------------
void moveLeft()
{
	int tmpMAP[MAX][MAX] = { 0 };

	for (int r = 1; r <= N; r++)
	{
		int index = 1; // 왼쪽부터 채워 넣을 위치

		// 1단계 : 0이 아닌 블록을 왼쪽으로 몰아 넣는다
		for (int c = 1; c <= N; c++)
		{
			if (ansMAP[r][c] == 0)
				continue;

			tmpMAP[r][index++] = ansMAP[r][c];
		}

		// 2단계 : 왼쪽부터 오른쪽 이웃과 비교해 합친다
		for (int c = 1; c <= N - 1; c++)
		{
			if (tmpMAP[r][c] != tmpMAP[r][c + 1])
				continue;

			if (tmpMAP[r][c] == 0)
				break;

			tmpMAP[r][c] *= 2;

			// 오른쪽에 남은 블록들을 왼쪽으로 한 칸씩 당긴다
			for (int tc = c + 1; tc <= N - 1; tc++)
				tmpMAP[r][tc] = tmpMAP[r][tc + 1];

			tmpMAP[r][N] = 0;
		}
	}

	copyMap(ansMAP, tmpMAP);
}

// --------------------------------------------------
// 현재 방향 수열대로 5번 이동시킨다
// --------------------------------------------------
void simulate()
{
	// 항상 원본에서 새로 시작해야 다음 수열이 오염되지 않는다
	copyMap(ansMAP, oMAP);

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
// 시뮬레이션 결과 보드에서 가장 큰 블록 값
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
// DFS : 길이 5의 방향 수열을 모두 만든다 (4^5 = 1024가지)
// --------------------------------------------------
void DFS(int depth)
{
	// 다섯 번의 방향이 다 정해지면 실제로 굴려 본다
	if (depth == 5)
	{
		// printCases();

		simulate();

		int tmp = findMax();
		if (maxAnswer < tmp)
			maxAnswer = tmp;

		return;
	}

	// 매 순번마다 네 방향 모두 시도 (중복 허용 = 중복순열)
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

		maxAnswer = 0;

		DFS(0);

		printf("%d\n", maxAnswer);
	}

	return 0;
}
