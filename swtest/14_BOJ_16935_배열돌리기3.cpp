/*
	[BOJ] 16935 - 배열 돌리기 3
	https://www.acmicpc.net/problem/16935

	■ 문제 요약
	  N x M 배열(N, M 모두 짝수)과 R개의 연산이 주어진다.
	  연산을 순서대로 모두 수행한 뒤 최종 배열을 출력한다.

	    1번 : 상하 반전
	    2번 : 좌우 반전
	    3번 : 시계 방향 90도 회전
	    4번 : 반시계 방향 90도 회전
	    5번 : 배열을 4분면으로 나눠 사분면 전체를 시계 방향으로 한 칸 이동
	    6번 : 배열을 4분면으로 나눠 사분면 전체를 반시계 방향으로 한 칸 이동

	  사분면 번호는 아래와 같다.

	      1 | 2
	      --+--
	      4 | 3

	■ 풀이 방침
	  연산을 하나씩 함수로 만들고 그대로 순서대로 호출한다.
	  모든 연산의 공통 골격은 같다.

	    (1) 현재 MAP을 temp로 복사한다
	    (2) temp에서 읽어 MAP에 쓴다

	  제자리(in-place)로 쓰면 아직 옮기지 않은 값을 이미 덮어써 버리기 때문에
	  스냅샷을 두고 "읽기는 temp, 쓰기는 MAP" 규칙을 지키는 것이 핵심이다.

	■ 주의할 점
	  3, 4번 회전은 배열의 가로/세로 크기가 서로 바뀐다 (N x M -> M x N).
	  그래서 회전 함수 안에서 N과 M을 먼저 swap한 뒤,
	  "새 좌표 (r, c)" 기준으로 식을 세워야 인덱스가 꼬이지 않는다.
	  swap 이후이므로 식에 등장하는 N, M은 이미 뒤바뀐 값이라는 점에 유의.
*/

#include <stdio.h>                // scanf, printf, putchar 사용

#define MAX (100 + 10)            // 배열 최대 크기 (100 + 여유 10)

// 명령 번호를 이름으로 정의 (main의 분기를 읽기 쉽게)
#define FILP_UPDOWN (1)           // 1번 : 상하 반전
#define FILP_LEFTRIGHT (2)        // 2번 : 좌우 반전
#define CLOCKWISE (3)             // 3번 : 시계 90도 회전
#define COUNTER_CLOCKWISE (4)     // 4번 : 반시계 90도 회전
#define SPILT_CLOCKWISE (5)       // 5번 : 사분면 시계 이동
#define SPILT_COUNTERWISE (6)     // 6번 : 사분면 반시계 이동

int N, M, R;          // N: 행 개수, M: 열 개수, R: 연산 개수
int MAP[MAX][MAX];    // 현재 배열, 1-based
int temp[MAX][MAX];   // 연산 직전 상태 스냅샷(읽기 전용으로 사용)

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &M, &R);       // 행 수, 열 수, 연산 수 입력

	for (int r = 1; r <= N; r++)         // 1행부터 N행까지
	{
		for (int c = 1; c <= M; c++)     // 1열부터 M열까지
		{
			scanf("%d", &MAP[r][c]);     // (r, c) 칸의 값 입력
		}
	}
}

// ---------------------------
// original -> copy 로 배열 전체 복사
//
// 주의: 복사 범위가 현재의 N, M을 따른다.
//       회전 함수에서는 반드시 "swap 하기 전"에 복사해야 원본 전체가 담긴다.
// ---------------------------
void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)         // 현재 행 수만큼
	{
		for (int c = 1; c <= M; c++)     // 현재 열 수만큼
		{
			copy[r][c] = original[r][c]; // 한 칸씩 그대로 복사
		}
	}
}

// ---------------------------
// 최종 배열 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)         // 모든 행에 대해
	{
		for (int c = 1; c <= M; c++)     // 한 행의 모든 칸을
			printf("%d ", MAP[r][c]);    // 공백으로 구분해 출력
		putchar('\n');                   // 한 행이 끝나면 줄바꿈
	}
}

// ---------------------------
// 1번 연산: 상하 반전
//
//   1 2        5 6
//   3 4   ->   3 4
//   5 6        1 2
//
//   r번째 행이 (N+1-r)번째 행 자리로 간다. 열은 그대로.
// ---------------------------
void filp_UpDown()
{
	copyMap(temp, MAP);                  // 연산 전 상태 스냅샷

	for (int r = 1; r <= N; r++)         // 결과 배열의 모든 행
	{
		for (int c = 1; c <= M; c++)     // 결과 배열의 모든 열
		{
			MAP[r][c] = temp[N + 1 - r][c];   // 위아래가 뒤집힌 행에서 값을 가져온다
		}
	}
}

// ---------------------------
// 2번 연산: 좌우 반전
//
//   1 2 3      3 2 1
//   4 5 6  ->  6 5 4
//
//   c번째 열이 (M+1-c)번째 열 자리로 간다. 행은 그대로.
// ---------------------------
void filp_LeftRight()
{
	copyMap(temp, MAP);                  // 연산 전 상태 스냅샷

	for (int r = 1; r <= N; r++)         // 결과 배열의 모든 행
	{
		for (int c = 1; c <= M; c++)     // 결과 배열의 모든 열
		{
			MAP[r][c] = temp[r][M + 1 - c];   // 좌우가 뒤집힌 열에서 값을 가져온다
		}
	}
}

// ---------------------------
// 3번 연산: 시계 방향 90도 회전
//
//   1 2        3 1
//   3 4   ->   4 2
//
//   N x M 배열이 M x N 배열이 되므로 N과 M을 먼저 교환한다.
//   교환 뒤의 M은 "원래의 N"이라는 점이 식의 핵심이다.
//
//   결과[r][c] = 원본[원래N + 1 - c][r] = temp[M + 1 - c][r]
// ---------------------------
void Clockwise()
{
	// 반드시 swap 전에 복사해야 원본 N x M 전체가 temp에 담긴다
	copyMap(temp, MAP);

	// 회전 후 행/열 크기 교환
	int t = N;                           // 원래 N을 잠시 저장
	N = M;                               // 새 행 수 = 원래 열 수
	M = t;                               // 새 열 수 = 원래 행 수

	for (int r = 1; r <= N; r++)         // 회전 후 배열의 모든 행
	{
		for (int c = 1; c <= M; c++)     // 회전 후 배열의 모든 열
		{
			MAP[r][c] = temp[M + 1 - c][r];   // 시계 90도 대응 좌표에서 값을 가져온다
		}
	}
}

// ---------------------------
// 4번 연산: 반시계 방향 90도 회전
//
//   1 2        2 4
//   3 4   ->   1 3
//
//   마찬가지로 N과 M이 교환된다.
//   교환 뒤의 N은 "원래의 M"이다.
//
//   결과[r][c] = 원본[c][원래M + 1 - r] = temp[c][N + 1 - r]
// ---------------------------
void counter_Clockwise()
{
	copyMap(temp, MAP);                  // swap 전에 원본 전체를 스냅샷

	// 회전 후 행/열 크기 교환
	int t = N;                           // 원래 N을 잠시 저장
	N = M;                               // 새 행 수 = 원래 열 수
	M = t;                               // 새 열 수 = 원래 행 수

	for (int r = 1; r <= N; r++)         // 회전 후 배열의 모든 행
	{
		for (int c = 1; c <= M; c++)     // 회전 후 배열의 모든 열
		{
			MAP[r][c] = temp[c][N + 1 - r];   // 반시계 90도 대응 좌표에서 값을 가져온다
		}
	}
}

// ---------------------------
// 5번 연산: 사분면을 통째로 시계 방향 한 칸 이동
//
//   1 | 2       이동 : 1 -> 2 -> 3 -> 4 -> 1
//   --+--
//   4 | 3
//
//   각 사분면은 halfN x halfM 크기이고 내부 배치는 그대로 유지된다.
//   사분면의 시작 좌표만 바꿔서 블록 단위로 통째로 옮기면 된다.
// ---------------------------
void split_Clockwise()
{
	copyMap(temp, MAP);                  // 연산 전 상태 스냅샷

	int sr1, sc1, sr2, sc2, sr3, sc3, sr4, sc4;   // 사분면 1~4의 시작 행/열
	int halfN, halfM;                    // 사분면 하나의 세로/가로 크기

	// N, M이 짝수이므로 정확히 반으로 나뉜다
	halfN = N / 2;                       // 사분면 세로 크기
	halfM = M / 2;                       // 사분면 가로 크기

	// 각 사분면의 시작(왼쪽 위) 좌표
	sr1 = 1;         sc1 = 1;            // 1사분면 (좌상)
	sr2 = 1;         sc2 = halfM + 1;    // 2사분면 (우상)
	sr3 = halfN + 1; sc3 = halfM + 1;    // 3사분면 (우하)
	sr4 = halfN + 1; sc4 = 1;            // 4사분면 (좌하)

	// 블록 안의 상대 좌표 (r, c)를 그대로 유지한 채 시작점만 바꿔 복사한다.
	// 읽기는 전부 temp라서 네 번의 복사 순서는 서로 영향을 주지 않는다.

	// 1 -> 2
	for (int r = 0; r < halfN; r++)      // 블록 안 상대 행
	{
		for (int c = 0; c < halfM; c++)  // 블록 안 상대 열
		{
			MAP[sr2 + r][sc2 + c] = temp[sr1 + r][sc1 + c];   // 1사분면 값을 2사분면 자리에
		}
	}

	// 2 -> 3
	for (int r = 0; r < halfN; r++)      // 블록 안 상대 행
	{
		for (int c = 0; c < halfM; c++)  // 블록 안 상대 열
		{
			MAP[sr3 + r][sc3 + c] = temp[sr2 + r][sc2 + c];   // 2사분면 값을 3사분면 자리에
		}
	}

	// 3 -> 4
	for (int r = 0; r < halfN; r++)      // 블록 안 상대 행
	{
		for (int c = 0; c < halfM; c++)  // 블록 안 상대 열
		{
			MAP[sr4 + r][sc4 + c] = temp[sr3 + r][sc3 + c];   // 3사분면 값을 4사분면 자리에
		}
	}

	// 4 -> 1
	for (int r = 0; r < halfN; r++)      // 블록 안 상대 행
	{
		for (int c = 0; c < halfM; c++)  // 블록 안 상대 열
		{
			MAP[sr1 + r][sc1 + c] = temp[sr4 + r][sc4 + c];   // 4사분면 값을 1사분면 자리에
		}
	}
}

// ---------------------------
// 6번 연산: 사분면을 통째로 반시계 방향 한 칸 이동
//
//   1 | 2       이동 : 2 -> 1 -> 4 -> 3 -> 2
//   --+--
//   4 | 3
//
//   5번 연산의 정확한 역방향.
// ---------------------------
void split_counterClockwise()
{
	copyMap(temp, MAP);                  // 연산 전 상태 스냅샷

	int sr1, sc1, sr2, sc2, sr3, sc3, sr4, sc4;   // 사분면 1~4의 시작 행/열
	int halfN, halfM;                    // 사분면 하나의 세로/가로 크기

	halfN = N / 2;                       // 사분면 세로 크기
	halfM = M / 2;                       // 사분면 가로 크기

	// 각 사분면의 시작(왼쪽 위) 좌표
	sr1 = 1;         sc1 = 1;            // 1사분면 (좌상)
	sr2 = 1;         sc2 = halfM + 1;    // 2사분면 (우상)
	sr3 = halfN + 1; sc3 = halfM + 1;    // 3사분면 (우하)
	sr4 = halfN + 1; sc4 = 1;            // 4사분면 (좌하)

	// 2 -> 1
	for (int r = 0; r < halfN; r++)      // 블록 안 상대 행
	{
		for (int c = 0; c < halfM; c++)  // 블록 안 상대 열
		{
			MAP[sr1 + r][sc1 + c] = temp[sr2 + r][sc2 + c];   // 2사분면 값을 1사분면 자리에
		}
	}

	// 3 -> 2
	for (int r = 0; r < halfN; r++)      // 블록 안 상대 행
	{
		for (int c = 0; c < halfM; c++)  // 블록 안 상대 열
		{
			MAP[sr2 + r][sc2 + c] = temp[sr3 + r][sc3 + c];   // 3사분면 값을 2사분면 자리에
		}
	}

	// 4 -> 3
	for (int r = 0; r < halfN; r++)      // 블록 안 상대 행
	{
		for (int c = 0; c < halfM; c++)  // 블록 안 상대 열
		{
			MAP[sr3 + r][sc3 + c] = temp[sr4 + r][sc4 + c];   // 4사분면 값을 3사분면 자리에
		}
	}

	// 1 -> 4
	for (int r = 0; r < halfN; r++)      // 블록 안 상대 행
	{
		for (int c = 0; c < halfM; c++)  // 블록 안 상대 열
		{
			MAP[sr4 + r][sc4 + c] = temp[sr1 + r][sc1 + c];   // 1사분면 값을 4사분면 자리에
		}
	}
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	input();                             // N, M, R, 배열 입력

	// R개의 연산을 입력받는 즉시 차례대로 수행
	for (int r = 0; r < R; r++)
	{
		int COMMAND;                     // 이번 연산 번호
		scanf("%d", &COMMAND);           // 연산 번호 입력

		if (COMMAND == FILP_UPDOWN)                  // 1번이면
			filp_UpDown();                           // 상하 반전
		else if (COMMAND == FILP_LEFTRIGHT)          // 2번이면
			filp_LeftRight();                        // 좌우 반전
		else if (COMMAND == CLOCKWISE)               // 3번이면
			Clockwise();                             // 시계 90도 회전
		else if (COMMAND == COUNTER_CLOCKWISE)       // 4번이면
			counter_Clockwise();                     // 반시계 90도 회전
		else if (COMMAND == SPILT_CLOCKWISE)         // 5번이면
			split_Clockwise();                       // 사분면 시계 이동
		else if (COMMAND == SPILT_COUNTERWISE)       // 6번이면
			split_counterClockwise();                // 사분면 반시계 이동
	}

	// 최종 배열 출력
	printMap();

	return 0;                            // 프로그램 정상 종료
}
