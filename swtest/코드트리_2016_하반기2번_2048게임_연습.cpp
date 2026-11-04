/*
	[코드트리] 2016 하반기 2번 - 2048 게임 (연습 버전)

	■ 요약
	  길이 5의 방향 수열(4^5 = 1024가지)을 DFS로 만들고, 수열마다 원본 보드를 복사해 5번 기울인다.
	  한 번 기울일 때는 "압축 -> 합치기 -> 당기기" 순서로 처리한다.
	  자세한 설명은 코드트리_2016_하반기2번_2048게임.cpp 참고.
*/
#include <stdio.h>               // scanf, printf, putchar 사용

#define MAX (20+5)	             // N 최대 20 + 여유

#define UP (0)                   // 위
#define RIGHT (1)                // 오른쪽
#define DOWN (2)                 // 아래
#define LEFT (3)                 // 왼쪽

int T;                           // 테스트 케이스 수

int N;                           // 보드 크기
int oMAP[MAX][MAX];              // 입력 원본 보드
int ansMAP[MAX][MAX];            // 시뮬레이션용 작업 보드

int num_of_cases[5 + 5];         // 방향 수열 (길이 5)

int maxAnswer;                   // 가장 큰 블록 값

void input()
{
	scanf("%d", &N);                     // 보드 크기 입력

	for (int r = 1; r <= N; r++)         // 모든 행
		for (int c = 1; c <= N; c++)     // 모든 열
			scanf("%d", &oMAP[r][c]);    // 블록 값 입력
}

void printMap(int map[MAX][MAX])
{
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= N; c++)     // 모든 열
			printf("%2d", map[r][c]);    // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)         // 모든 행
		for (int c = 1; c <= N; c++)     // 모든 열
			copy[r][c] = original[r][c]; // 한 칸씩 복사
}

void printCases()
{
	for (int i = 0; i < 5; i++) printf("%d ", num_of_cases[i]);   // 방향 수열 출력
	putchar('\n');                       // 줄바꿈
}


void moveUp()
{
	int tmpMAP[MAX][MAX] = { 0 };        // 결과 보드

	for (int c = 1; c <= N; c++)         // 열마다
	{
		int index = 1;                   // 위에서부터 채울 위치
		for (int r = 1; r <= N; r++)     // 위에서 아래로
		{
			if (ansMAP[r][c] == 0) continue;   // 빈칸은 건너뜀

			tmpMAP[index++][c] = ansMAP[r][c]; // 위로 압축
		}
		for (int r = 1; r <= N - 1; r++) // 위에서부터 이웃 비교
		{
			if (tmpMAP[r][c] != tmpMAP[r + 1][c]) continue;   // 다르면 합칠 수 없다
			if (tmpMAP[r][c] == 0) break;                     // 빈칸이면 끝

			tmpMAP[r][c] *= 2;           // 합치기

			for (int tr = r + 1; tr <= N - 1; tr++)
				tmpMAP[tr][c] = tmpMAP[tr + 1][c];   // 뒤쪽 블록 당기기

			tmpMAP[N][c] = 0;            // 맨 아래 비우기
		}
	}

	copyMap(ansMAP, tmpMAP);             // 결과 반영
}

void moveLeft()
{
	int tmpMAP[MAX][MAX] = { 0 };        // 결과 보드

	for (int r = 1; r <= N; r++)         // 행마다
	{
		int index = 1;                   // 왼쪽부터 채울 위치
		for (int c = 1; c <= N; c++)     // 왼쪽에서 오른쪽으로
		{
			if (ansMAP[r][c] == 0) continue;   // 빈칸은 건너뜀

			tmpMAP[r][index++] = ansMAP[r][c]; // 왼쪽으로 압축
		}


		for (int c = 1; c <= N - 1; c++) // 왼쪽부터 이웃 비교
		{
			if (tmpMAP[r][c] != tmpMAP[r][c + 1]) continue;   // 다르면 합칠 수 없다
			if (tmpMAP[r][c] == 0) break;                     // 빈칸이면 끝

			tmpMAP[r][c] *= 2;           // 합치기
			for (int tc = c + 1; tc <= N - 1; tc++)
				tmpMAP[r][tc] = tmpMAP[r][tc + 1];   // 뒤쪽 블록 당기기

			tmpMAP[r][N] = 0;            // 맨 오른쪽 비우기
		}
	}
	copyMap(ansMAP, tmpMAP);             // 결과 반영
}

void moveRight()
{
	int tmpMAP[MAX][MAX] = { 0 };        // 결과 보드

	for (int r = 1; r <= N; r++)         // 행마다
	{
		int index = N;                   // 오른쪽부터 채울 위치
		for (int c = N; c >= 1; c--)     // 오른쪽에서 왼쪽으로
		{
			if (ansMAP[r][c] == 0) continue;   // 빈칸은 건너뜀

			tmpMAP[r][index--] = ansMAP[r][c]; // 오른쪽으로 압축
		}

		for (int c = N; c >= 2; c--)     // 오른쪽부터 이웃 비교
		{
			if (tmpMAP[r][c] != tmpMAP[r][c - 1]) continue;   // 다르면 합칠 수 없다
			if (tmpMAP[r][c] == 0) break;                     // 빈칸이면 끝

			tmpMAP[r][c] *= 2;           // 합치기

			for (int tc = c - 1; tc >= 2; tc--)
				tmpMAP[r][tc] = tmpMAP[r][tc - 1];   // 뒤쪽 블록 당기기

			tmpMAP[r][1] = 0;            // 맨 왼쪽 비우기
		}
	}

	copyMap(ansMAP, tmpMAP);             // 결과 반영
}

void moveDown()
{
	int tmpMAP[MAX][MAX] = { 0 };        // 결과 보드

	for (int c = 1; c <= N; c++)         // 열마다
	{
		int index = N;                   // 아래부터 채울 위치
		for (int r = N; r >= 1; r--)     // 아래에서 위로
		{
			if (ansMAP[r][c] == 0) continue;   // 빈칸은 건너뜀

			tmpMAP[index--][c] = ansMAP[r][c]; // 아래로 압축
		}

		for (int r = N; r >= 2; r--)     // 아래부터 이웃 비교
		{
			if (tmpMAP[r][c] != tmpMAP[r - 1][c]) continue;   // 다르면 합칠 수 없다
			if (tmpMAP[r][c] == 0) break;                     // 빈칸이면 끝

			tmpMAP[r][c] *= 2;           // 합치기

			for (int tr = r - 1; tr >= 2; tr--)
				tmpMAP[tr][c] = tmpMAP[tr - 1][c];   // 뒤쪽 블록 당기기

			tmpMAP[1][c] = 0;            // 맨 위 비우기
		}
	}

	copyMap(ansMAP, tmpMAP);             // 결과 반영
}

void simulate()
{
	copyMap(ansMAP, oMAP);               // 원본에서 새로 시작

	for (int i = 0; i < 5; i++)          // 5번 이동
	{
		int direction = num_of_cases[i]; // 이번 방향

		if (direction == UP) moveUp();               // 위
		else if (direction == RIGHT) moveRight();    // 오른쪽
		else if (direction == LEFT) moveLeft();      // 왼쪽
		else if (direction == DOWN) moveDown();      // 아래
	}
}

int findMax()
{
	int maxValue = 0;                    // 가장 큰 값
	for (int r = 1; r <= N; r++)         // 모든 행
		for (int c = 1; c <= N; c++)     // 모든 열
			if (maxValue < ansMAP[r][c]) maxValue = ansMAP[r][c];   // 최댓값 갱신

	return maxValue;                     // 반환
}

void DFS(int depth)
{
	if (depth == 5)                      // 수열 완성
	{
		//printCases();

		simulate();                      // 5번 이동

		int tmp = findMax();             // 결과 최댓값
		if (maxAnswer < tmp) maxAnswer = tmp;   // 답 갱신

		return;                          // 되돌아간다
	}
	for (int i = 0; i < 4; i++)          // 4방향 모두
	{
		num_of_cases[depth] = i;         // 이번 방향 기록

		DFS(depth + 1);                  // 다음 순번
	}
}

int main()
{
	//scanf("%d", &T);

	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		maxAnswer = 0;                   // 답 초기화

		DFS(0);                          // 수열 탐색

		printf("%d\n", maxAnswer);       // 결과 출력
	}

	return 0;                            // 프로그램 정상 종료
}
