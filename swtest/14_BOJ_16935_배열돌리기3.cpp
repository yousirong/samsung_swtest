// BOJ 16935 - 배열 돌리기 3
// https://www.acmicpc.net/problem/16935
// 문제 요약
// 배열에 대해 총 6가지 연산이 주어진다.
// 이 연산들을 순서대로 수행한 뒤 최종 배열을 출력하는 문제이다.
//
// 연산 종류
// 1. 상하 반전
// 2. 좌우 반전
// 3. 시계 방향 90도 회전
// 4. 반시계 방향 90도 회전
// 5. 배열을 4분면으로 나눈 뒤 시계 방향으로 이동
// 6. 배열을 4분면으로 나눈 뒤 반시계 방향으로 이동
//
// 핵심 아이디어
// - 회전/반전 과정에서 값을 바로 덮어쓰면 원본 값이 사라질 수 있다.
// - 그래서 항상 temp에 현재 MAP을 복사한 뒤,
//   temp를 기준으로 읽고 MAP에 결과를 쓴다.

#include <stdio.h>

#define MAX (100 + 10)

// 명령 번호를 상수로 정의
#define FILP_UPDOWN (1)
#define FILP_LEFTRIGHT (2)
#define CLOCKWISE (3)
#define COUNTER_CLOCKWISE (4)
#define SPILT_CLOCKWISE (5)
#define SPILT_COUNTERWISE (6)

int N, M, R;          // N: 행 개수, M: 열 개수, R: 명령 개수
int MAP[MAX][MAX];    // 현재 배열
int temp[MAX][MAX];   // 원본 복사용 임시 배열

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &M, &R);

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}
}

// ---------------------------
// 배열 복사 함수
// copy 배열에 original 배열을 그대로 복사
// ---------------------------
void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			copy[r][c] = original[r][c];
		}
	}
}

// ---------------------------
// 최종 배열 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
}

// ---------------------------
// 1번 연산: 상하 반전
//
// 위쪽 행과 아래쪽 행을 뒤집는다.
//
// 예)
// 1 2      5 6
// 3 4  ->  3 4
// 5 6      1 2
//
// 공식:
// MAP[r][c] = temp[N+1-r][c]
// ---------------------------
void filp_UpDown()
{
	copyMap(temp, MAP);

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			MAP[r][c] = temp[N + 1 - r][c];
		}
	}
}

// ---------------------------
// 2번 연산: 좌우 반전
//
// 왼쪽 열과 오른쪽 열을 뒤집는다.
//
// 예)
// 1 2 3      3 2 1
// 4 5 6  ->  6 5 4
//
// 공식:
// MAP[r][c] = temp[r][M+1-c]
// ---------------------------
void filp_LeftRight()
{
	copyMap(temp, MAP);

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			MAP[r][c] = temp[r][M + 1 - c];
		}
	}
}

// ---------------------------
// 3번 연산: 시계 방향 90도 회전
//
// 예)
// 1 2      3 1
// 3 4  ->  4 2
//
// 회전 후에는 행과 열의 크기가 서로 바뀐다.
// 즉, N x M 배열이 M x N 배열이 된다.
//
// 공식:
// MAP[r][c] = temp[M+1-c][r]
//
// 주의:
// N과 M을 먼저 바꿔야 한다.
// ---------------------------
void Clockwise()
{
	copyMap(temp, MAP);

	// 회전 후 행/열 크기 교환
	int t = N;
	N = M;
	M = t;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			MAP[r][c] = temp[M + 1 - c][r];
		}
	}
}

// ---------------------------
// 4번 연산: 반시계 방향 90도 회전
//
// 예)
// 1 2      2 4
// 3 4  ->  1 3
//
// 역시 회전 후 행과 열의 크기가 서로 바뀐다.
//
// 공식:
// MAP[r][c] = temp[c][N+1-r]
//
// 여기서 N, M은 이미 swap 이후 값 기준
// ---------------------------
void counter_Clockwise()
{
	copyMap(temp, MAP);

	// 회전 후 행/열 크기 교환
	int t = N;
	N = M;
	M = t;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			MAP[r][c] = temp[c][N + 1 - r];
		}
	}
}

// ---------------------------
// 5번 연산: 4분면을 시계 방향으로 이동
//
// 배열을 다음처럼 4분면으로 나눈다.
//
// 1 | 2
// --+--
// 4 | 3
//
// 이동 방향:
// 4 -> 1
// 1 -> 2
// 2 -> 3
// 3 -> 4
//
// 즉, 각 사분면 전체를 시계 방향으로 한 칸씩 밀어 넣는다.
// ---------------------------
void split_Clockwise()
{
	copyMap(temp, MAP);

	int sr1, sc1, sr2, sc2, sr3, sc3, sr4, sc4;
	int halfN, halfM;

	// 절반 크기
	halfN = N / 2;
	halfM = M / 2;

	// 1사분면 (좌상)
	sr1 = 1;
	sc1 = 1;

	// 2사분면 (우상)
	sr2 = 1;
	sc2 = halfM + 1;

	// 3사분면 (우하)
	sr3 = halfN + 1;
	sc3 = halfM + 1;

	// 4사분면 (좌하)
	sr4 = halfN + 1;
	sc4 = 1;

	// 1 -> 2
	for (int r = 0; r < halfN; r++)
	{
		for (int c = 0; c < halfM; c++)
		{
			MAP[sr2 + r][sc2 + c] = temp[sr1 + r][sc1 + c];
		}
	}

	// 2 -> 3
	for (int r = 0; r < halfN; r++)
	{
		for (int c = 0; c < halfM; c++)
		{
			MAP[sr3 + r][sc3 + c] = temp[sr2 + r][sc2 + c];
		}
	}

	// 3 -> 4
	for (int r = 0; r < halfN; r++)
	{
		for (int c = 0; c < halfM; c++)
		{
			MAP[sr4 + r][sc4 + c] = temp[sr3 + r][sc3 + c];
		}
	}

	// 4 -> 1
	for (int r = 0; r < halfN; r++)
	{
		for (int c = 0; c < halfM; c++)
		{
			MAP[sr1 + r][sc1 + c] = temp[sr4 + r][sc4 + c];
		}
	}
}

// ---------------------------
// 6번 연산: 4분면을 반시계 방향으로 이동
//
// 배열을 다음처럼 4분면으로 나눈다.
//
// 1 | 2
// --+--
// 4 | 3
//
// 이동 방향:
// 2 -> 1
// 3 -> 2
// 4 -> 3
// 1 -> 4
//
// 즉, 각 사분면 전체를 반시계 방향으로 한 칸씩 민다.
// ---------------------------
void split_counterClockwise()
{
	copyMap(temp, MAP);

	int sr1, sc1, sr2, sc2, sr3, sc3, sr4, sc4;
	int halfN, halfM;

	halfN = N / 2;
	halfM = M / 2;

	// 1사분면 (좌상)
	sr1 = 1;
	sc1 = 1;

	// 2사분면 (우상)
	sr2 = 1;
	sc2 = halfM + 1;

	// 3사분면 (우하)
	sr3 = halfN + 1;
	sc3 = halfM + 1;

	// 4사분면 (좌하)
	sr4 = halfN + 1;
	sc4 = 1;

	// 2 -> 1
	for (int r = 0; r < halfN; r++)
	{
		for (int c = 0; c < halfM; c++)
		{
			MAP[sr1 + r][sc1 + c] = temp[sr2 + r][sc2 + c];
		}
	}

	// 3 -> 2
	for (int r = 0; r < halfN; r++)
	{
		for (int c = 0; c < halfM; c++)
		{
			MAP[sr2 + r][sc2 + c] = temp[sr3 + r][sc3 + c];
		}
	}

	// 4 -> 3
	for (int r = 0; r < halfN; r++)
	{
		for (int c = 0; c < halfM; c++)
		{
			MAP[sr3 + r][sc3 + c] = temp[sr4 + r][sc4 + c];
		}
	}

	// 1 -> 4
	for (int r = 0; r < halfN; r++)
	{
		for (int c = 0; c < halfM; c++)
		{
			MAP[sr4 + r][sc4 + c] = temp[sr1 + r][sc1 + c];
		}
	}
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();

	// R개의 명령을 차례대로 수행
	for (int r = 0; r < R; r++)
	{
		int COMMAND;
		scanf("%d", &COMMAND);

		if (COMMAND == FILP_UPDOWN)
			filp_UpDown();
		else if (COMMAND == FILP_LEFTRIGHT)
			filp_LeftRight();
		else if (COMMAND == CLOCKWISE)
			Clockwise();
		else if (COMMAND == COUNTER_CLOCKWISE)
			counter_Clockwise();
		else if (COMMAND == SPILT_CLOCKWISE)
			split_Clockwise();
		else if (COMMAND == SPILT_COUNTERWISE)
			split_counterClockwise();
	}

	// 최종 배열 출력
	printMap();

	return 0;
}