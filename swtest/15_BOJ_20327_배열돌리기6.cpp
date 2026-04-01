// BOJ 20327 - 배열 돌리기 6
// https://www.acmicpc.net/problem/20327
// 문제 요약
// 크기가 2^N x 2^N 인 배열이 주어지고,
// R개의 연산을 순서대로 수행한 뒤 결과를 출력하는 문제이다.
//
// 각 연산은 "크기 2^level x 2^level" 블록 기준으로 동작한다.
//
// 연산 종류
// 1: 각 블록 내부를 상하 반전
// 2: 각 블록 내부를 좌우 반전
// 3: 각 블록 내부를 시계 방향 90도 회전
// 4: 각 블록 내부를 반시계 방향 90도 회전
// 5: 블록들의 위치만 상하 반전
// 6: 블록들의 위치만 좌우 반전
// 7: 블록들의 위치만 시계 방향 90도 회전
// 8: 블록들의 위치만 반시계 방향 90도 회전
//
// 핵심 아이디어
// - 1~4번은 "블록 내부"를 직접 변형한다.
// - 5~8번은 블록 내부는 그대로 두고 "블록 위치만" 바꿔야 한다.
// - 이 코드는
//     전체 배열 변환 + 각 블록 역변환
//   을 조합해서 "블록 위치만 바뀐 효과"를 만든다.
//
// 예를 들어 5번(블록 위치 상하 반전)은
// 1) 전체 배열을 상하 반전
// 2) 각 블록도 다시 상하 반전
// 하면
// - 블록 내부 방향은 원상복구되고
// - 블록 위치만 위아래로 바뀐다.

#include <stdio.h>

#define MAX (128 + 10)

// 명령 번호
#define FLIP_UPDOWN (1)
#define FLIP_LEFTRIGHT (2)
#define CLOCKWISE (3)
#define COUNTER_CLOCKWISE (4)
#define SPLIT_UPDOWN (5)
#define SPLIT_LEFTRIGHT (6)
#define SPLIT_CLOCKWISE (7)
#define SPLIT_COUNTER_CLOCKWISE (8)

int N, R, S;          // N: 지수, R: 연산 개수, S = 2^N = 실제 배열 한 변 길이
int MAP[MAX][MAX];    // 실제 배열
int temp[MAX][MAX];   // 블록 복사용 임시 배열

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &N, &R);

	// 실제 배열 크기 S = 2^N
	S = (1 << N);

	// S x S 배열 입력
	for (int r = 1; r <= S; r++)
		for (int c = 1; c <= S; c++)
			scanf("%d", &MAP[r][c]);
}

// ---------------------------
// 전체 배열 복사 함수
// ---------------------------
void copyMAP(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= S; r++)
		for (int c = 1; c <= S; c++)
			copy[r][c] = original[r][c];
}

// ---------------------------
// 배열 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= S; r++)
	{
		for (int c = 1; c <= S; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
}

// --------------------------------------------------
// 1번 연산: 블록 내부 상하 반전
// --------------------------------------------------

// 하나의 size x size 블록만 상하 반전
// 시작 좌표: (sr, sc)
void flipUpDownDivide(int sr, int sc, int size)
{
	// 현재 블록을 temp에 복사
	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			temp[r][c] = MAP[sr + r][sc + c];

	// 위아래 뒤집어서 다시 MAP에 반영
	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			MAP[sr + r][sc + c] = temp[size - 1 - r][c];
}

// 크기 2^level 블록들 각각에 대해 상하 반전 수행
void flipUpDown(int level)
{
	int divide = (1 << level);   // 블록 한 변 크기

	// 블록 시작점들을 순회
	for (int r = 1; r <= S; r += divide)
		for (int c = 1; c <= S; c += divide)
			flipUpDownDivide(r, c, divide);
}

// --------------------------------------------------
// 2번 연산: 블록 내부 좌우 반전
// --------------------------------------------------

// 하나의 size x size 블록만 좌우 반전
void flipleftrightdivide(int sr, int sc, int size)
{
	// 현재 블록 복사
	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			temp[r][c] = MAP[sr + r][sc + c];

	// 좌우 뒤집기
	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			MAP[sr + r][sc + c] = temp[r][size - 1 - c];
}

// 크기 2^level 블록들 각각 좌우 반전
void flipLeftRight(int level)
{
	int divide = (1 << level);

	for (int r = 1; r <= S; r += divide)
		for (int c = 1; c <= S; c += divide)
			flipleftrightdivide(r, c, divide);
}

// --------------------------------------------------
// 3번 연산: 블록 내부 시계 방향 90도 회전
// --------------------------------------------------

// 하나의 size x size 블록만 시계 방향 회전
void clockwisedivide(int sr, int sc, int size)
{
	// 현재 블록 복사
	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			temp[r][c] = MAP[sr + r][sc + c];

	// 시계 방향 90도 회전
	// new[r][c] = old[size-1-c][r]
	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			MAP[sr + r][sc + c] = temp[size - 1 - c][r];
}

// 크기 2^level 블록들 각각 시계 회전
void clockwise(int level)
{
	int divide = (1 << level);

	for (int r = 1; r <= S; r += divide)
		for (int c = 1; c <= S; c += divide)
			clockwisedivide(r, c, divide);
}

// --------------------------------------------------
// 4번 연산: 블록 내부 반시계 방향 90도 회전
// --------------------------------------------------

// 하나의 size x size 블록만 반시계 방향 회전
void counterclockwisedivide(int sr, int sc, int size)
{
	// 현재 블록 복사
	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			temp[r][c] = MAP[sr + r][sc + c];

	// 반시계 방향 90도 회전
	// new[r][c] = old[c][size-1-r]
	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			MAP[sr + r][sc + c] = temp[c][size - 1 - r];
}

// 크기 2^level 블록들 각각 반시계 회전
void counterClockwise(int level)
{
	int divide = (1 << level);

	for (int r = 1; r <= S; r += divide)
		for (int c = 1; c <= S; c += divide)
			counterclockwisedivide(r, c, divide);
}

// --------------------------------------------------
// 5~8번 연산: 블록 위치만 바꾸기
// --------------------------------------------------
//
// 이 문제의 핵심 트릭:
// 전체 배열을 변환하면 블록 위치도 같이 바뀌지만,
// 블록 내부도 함께 뒤집히거나 회전된다.
// 그래서 각 블록에 "반대 변환"을 한 번 더 적용하면
// 블록 내부는 원래대로 돌아가고, 위치만 바뀐 효과가 남는다.

// 5번 연산: 블록들의 위치만 상하 반전
//
// 방법:
// 1) 전체 배열을 상하 반전
// 2) 각 블록도 다시 상하 반전
//
// 결과:
// - 블록 내부는 원래 모양 유지
// - 블록 위치만 위아래 교환
void splitUpDown(int level)
{
	flipUpDown(N);      // 전체 배열 상하 반전
	flipUpDown(level);  // 각 블록도 상하 반전해서 내부 복구
}

// 6번 연산: 블록들의 위치만 좌우 반전
//
// 방법:
// 1) 전체 배열을 좌우 반전
// 2) 각 블록도 다시 좌우 반전
//
// 결과:
// - 블록 내부는 유지
// - 블록 위치만 좌우 교환
void splitLeftRight(int level)
{
	flipLeftRight(N);      // 전체 배열 좌우 반전
	flipLeftRight(level);  // 각 블록도 좌우 반전해서 내부 복구
}

// 7번 연산: 블록들의 위치만 시계 방향 회전
//
// 방법:
// 1) 전체 배열을 시계 방향 회전
// 2) 각 블록은 반시계 방향으로 되돌림
//
// 결과:
// - 블록 내부는 원래 모양
// - 블록 위치만 시계 방향 이동
void splitClockwise(int level)
{
	clockwise(N);             // 전체 배열 시계 회전
	counterClockwise(level);  // 각 블록 내부 복구
}

// 8번 연산: 블록들의 위치만 반시계 방향 회전
//
// 방법:
// 1) 전체 배열을 반시계 방향 회전
// 2) 각 블록은 시계 방향으로 되돌림
//
// 결과:
// - 블록 내부는 원래 모양
// - 블록 위치만 반시계 방향 이동
void splitCounterClockwise(int level)
{
	counterClockwise(N);  // 전체 배열 반시계 회전
	clockwise(level);     // 각 블록 내부 복구
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();

	// R개의 명령을 순서대로 수행
	for (int r = 0; r < R; r++)
	{
		int COMMAND, level;
		scanf("%d %d", &COMMAND, &level);

		if (COMMAND == FLIP_UPDOWN)
			flipUpDown(level);
		else if (COMMAND == FLIP_LEFTRIGHT)
			flipLeftRight(level);
		else if (COMMAND == CLOCKWISE)
			clockwise(level);
		else if (COMMAND == COUNTER_CLOCKWISE)
			counterClockwise(level);
		else if (COMMAND == SPLIT_UPDOWN)
			splitUpDown(level);
		else if (COMMAND == SPLIT_LEFTRIGHT)
			splitLeftRight(level);
		else if (COMMAND == SPLIT_CLOCKWISE)
			splitClockwise(level);
		else if (COMMAND == SPLIT_COUNTER_CLOCKWISE)
			splitCounterClockwise(level);
	}

	// 최종 결과 출력
	printMap();

	return 0;
}