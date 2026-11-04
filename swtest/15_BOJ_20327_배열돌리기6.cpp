/*
	[BOJ] 20327 - 배열 돌리기 6
	https://www.acmicpc.net/problem/20327

	■ 문제 요약
	  크기가 2^N x 2^N 인 배열과 R개의 연산이 주어진다.
	  각 연산은 (연산 번호, level) 형태이며, 배열을 한 변이 2^level 인 블록들로 잘라서 처리한다.

	    1 : 각 블록 "내부"를 상하 반전
	    2 : 각 블록 "내부"를 좌우 반전
	    3 : 각 블록 "내부"를 시계 방향 90도 회전
	    4 : 각 블록 "내부"를 반시계 방향 90도 회전
	    5 : 블록 내부는 그대로 두고 "블록들의 위치"만 상하 반전
	    6 : 블록 내부는 그대로 두고 "블록들의 위치"만 좌우 반전
	    7 : 블록 내부는 그대로 두고 "블록들의 위치"만 시계 방향 90도 회전
	    8 : 블록 내부는 그대로 두고 "블록들의 위치"만 반시계 방향 90도 회전

	  모든 연산 수행 후 최종 배열을 출력한다.

	■ 풀이 방침 (1 ~ 4번)
	  배열을 2^level 크기의 블록으로 나눠 각 블록마다 반전/회전을 그대로 적용한다.
	  블록 시작점은 (1, 1)에서 2^level 씩 건너뛰며 순회하면 된다.

	■ 풀이 방침 (5 ~ 8번) — 이 문제의 핵심 트릭
	  "블록 위치만 바꾸기"를 직접 구현하려면 인덱스 계산이 꽤 번거롭다.
	  대신 아래처럼 두 번의 변환을 겹쳐서 만든다.

	    (1) 전체 배열(= level N짜리 블록 하나)에 그 변환을 적용한다
	        → 블록 위치도 바뀌지만, 각 블록 내부까지 같이 뒤집혀 버린다
	    (2) 각 2^level 블록에 "되돌리는 변환"을 한 번 더 적용한다
	        → 블록 내부만 원래 모양으로 복구되고, 위치 변화만 남는다

	  되돌리는 변환은 다음과 같다.
	    상하 반전 <-> 상하 반전 (자기 자신이 역연산)
	    좌우 반전 <-> 좌우 반전 (자기 자신이 역연산)
	    시계 회전 <-> 반시계 회전

	■ 주의할 점
	  level == N 이면 블록이 하나뿐이라 5~8번은 아무 변화도 없어야 하는데,
	  위 방식은 같은 변환을 두 번(정방향 + 역방향) 적용하므로 자동으로 제자리가 된다.
*/

#include <stdio.h>                    // scanf, printf, putchar 사용

#define MAX (128 + 10)                // 최대 크기 2^7 = 128 + 여유

// 명령 번호를 이름으로 정의
#define FLIP_UPDOWN (1)               // 1번 : 블록 내부 상하 반전
#define FLIP_LEFTRIGHT (2)            // 2번 : 블록 내부 좌우 반전
#define CLOCKWISE (3)                 // 3번 : 블록 내부 시계 회전
#define COUNTER_CLOCKWISE (4)         // 4번 : 블록 내부 반시계 회전
#define SPLIT_UPDOWN (5)              // 5번 : 블록 위치 상하 반전
#define SPLIT_LEFTRIGHT (6)           // 6번 : 블록 위치 좌우 반전
#define SPLIT_CLOCKWISE (7)           // 7번 : 블록 위치 시계 회전
#define SPLIT_COUNTER_CLOCKWISE (8)   // 8번 : 블록 위치 반시계 회전

int N, R, S;          // N: 지수, R: 연산 개수, S = 2^N (배열 한 변의 실제 길이)
int MAP[MAX][MAX];    // 실제 배열, 1-based
int temp[MAX][MAX];   // 블록 하나를 잠시 담아 두는 버퍼, 0-based로 사용

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d", &N, &R);              // 지수 N과 연산 개수 R 입력

	// 실제 배열 크기 S = 2^N
	S = (1 << N);

	// S x S 배열 입력
	for (int r = 1; r <= S; r++)         // 모든 행
		for (int c = 1; c <= S; c++)     // 모든 열
			scanf("%d", &MAP[r][c]);     // (r, c) 칸의 값 입력
}

// ---------------------------
// 배열 전체 복사 (이 풀이에서는 블록 단위 복사만 쓰므로 실제로는 호출하지 않는다)
// ---------------------------
void copyMAP(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= S; r++)         // 모든 행
		for (int c = 1; c <= S; c++)     // 모든 열
			copy[r][c] = original[r][c]; // 한 칸씩 복사
}

// ---------------------------
// 최종 배열 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= S; r++)         // 모든 행에 대해
	{
		for (int c = 1; c <= S; c++)     // 한 행의 모든 칸을
			printf("%d ", MAP[r][c]);    // 공백으로 구분해 출력
		putchar('\n');                   // 한 행이 끝나면 줄바꿈
	}
}

/* --------------------------------------------------
   1번 연산: 블록 내부 상하 반전

   아래 네 쌍의 divide 함수는 전부 같은 골격이다.
     (1) MAP의 블록을 temp에 0-based로 떠 놓는다
     (2) temp에서 읽어 변환한 좌표로 MAP에 되쓴다
   블록마다 temp를 재사용하지만, 한 블록 처리가 끝난 뒤 다음 블록으로 넘어가므로 안전하다.
   -------------------------------------------------- */

// (sr, sc)에서 시작하는 size x size 블록 하나를 상하 반전
void flipUpDownDivide(int sr, int sc, int size)
{
	// 블록을 temp[0..size-1][0..size-1]로 복사
	for (int r = 0; r < size; r++)       // 블록 안 상대 행
		for (int c = 0; c < size; c++)   // 블록 안 상대 열
			temp[r][c] = MAP[sr + r][sc + c];   // 블록 값을 0-based로 떠 놓기

	// 행 인덱스를 뒤집어 되쓴다 : r <- size-1-r
	for (int r = 0; r < size; r++)       // 블록 안 상대 행
		for (int c = 0; c < size; c++)   // 블록 안 상대 열
			MAP[sr + r][sc + c] = temp[size - 1 - r][c];   // 위아래 뒤집힌 행에서 가져온다
}

// 한 변이 2^level 인 모든 블록에 대해 내부 상하 반전
void flipUpDown(int level)
{
	int divide = (1 << level);   // 블록 한 변 크기

	// 블록의 왼쪽 위 좌표만 골라 순회한다
	for (int r = 1; r <= S; r += divide)        // 블록 시작 행
		for (int c = 1; c <= S; c += divide)    // 블록 시작 열
			flipUpDownDivide(r, c, divide);     // 그 블록 하나를 상하 반전
}

// --------------------------------------------------
// 2번 연산: 블록 내부 좌우 반전
// --------------------------------------------------

// 블록 하나를 좌우 반전
void flipleftrightdivide(int sr, int sc, int size)
{
	for (int r = 0; r < size; r++)       // 블록 안 상대 행
		for (int c = 0; c < size; c++)   // 블록 안 상대 열
			temp[r][c] = MAP[sr + r][sc + c];   // 블록 값을 0-based로 떠 놓기

	// 열 인덱스를 뒤집어 되쓴다 : c <- size-1-c
	for (int r = 0; r < size; r++)       // 블록 안 상대 행
		for (int c = 0; c < size; c++)   // 블록 안 상대 열
			MAP[sr + r][sc + c] = temp[r][size - 1 - c];   // 좌우 뒤집힌 열에서 가져온다
}

// 한 변이 2^level 인 모든 블록에 대해 내부 좌우 반전
void flipLeftRight(int level)
{
	int divide = (1 << level);           // 블록 한 변 크기

	for (int r = 1; r <= S; r += divide)        // 블록 시작 행
		for (int c = 1; c <= S; c += divide)    // 블록 시작 열
			flipleftrightdivide(r, c, divide);  // 그 블록 하나를 좌우 반전
}

// --------------------------------------------------
// 3번 연산: 블록 내부 시계 방향 90도 회전
// --------------------------------------------------

// 블록 하나를 시계 방향 90도 회전
void clockwisedivide(int sr, int sc, int size)
{
	for (int r = 0; r < size; r++)       // 블록 안 상대 행
		for (int c = 0; c < size; c++)   // 블록 안 상대 열
			temp[r][c] = MAP[sr + r][sc + c];   // 블록 값을 0-based로 떠 놓기

	// 시계 90도 : new[r][c] = old[size-1-c][r]
	for (int r = 0; r < size; r++)       // 블록 안 상대 행
		for (int c = 0; c < size; c++)   // 블록 안 상대 열
			MAP[sr + r][sc + c] = temp[size - 1 - c][r];   // 시계 회전 대응 좌표에서 가져온다
}

// 한 변이 2^level 인 모든 블록에 대해 내부 시계 회전
void clockwise(int level)
{
	int divide = (1 << level);           // 블록 한 변 크기

	for (int r = 1; r <= S; r += divide)        // 블록 시작 행
		for (int c = 1; c <= S; c += divide)    // 블록 시작 열
			clockwisedivide(r, c, divide);      // 그 블록 하나를 시계 회전
}

// --------------------------------------------------
// 4번 연산: 블록 내부 반시계 방향 90도 회전
// --------------------------------------------------

// 블록 하나를 반시계 방향 90도 회전
void counterclockwisedivide(int sr, int sc, int size)
{
	for (int r = 0; r < size; r++)       // 블록 안 상대 행
		for (int c = 0; c < size; c++)   // 블록 안 상대 열
			temp[r][c] = MAP[sr + r][sc + c];   // 블록 값을 0-based로 떠 놓기

	// 반시계 90도 : new[r][c] = old[c][size-1-r]
	for (int r = 0; r < size; r++)       // 블록 안 상대 행
		for (int c = 0; c < size; c++)   // 블록 안 상대 열
			MAP[sr + r][sc + c] = temp[c][size - 1 - r];   // 반시계 회전 대응 좌표에서 가져온다
}

// 한 변이 2^level 인 모든 블록에 대해 내부 반시계 회전
void counterClockwise(int level)
{
	int divide = (1 << level);           // 블록 한 변 크기

	for (int r = 1; r <= S; r += divide)          // 블록 시작 행
		for (int c = 1; c <= S; c += divide)      // 블록 시작 열
			counterclockwisedivide(r, c, divide); // 그 블록 하나를 반시계 회전
}

/* --------------------------------------------------
   5 ~ 8번 연산: 블록 "위치"만 바꾸기

   전체 배열에 변환을 걸면 블록 위치는 원하는 대로 바뀌지만
   블록 내부까지 같이 변형된다. 그래서 각 블록에 역변환을 한 번 더 걸어
   내부만 원상 복구시키고 위치 변화만 남긴다.

   전체 배열 = "한 변이 2^N 인 블록 하나"이므로 level 자리에 N을 넣으면 된다.
   -------------------------------------------------- */

// 5번: 블록들의 위치만 상하 반전
//      상하 반전은 자기 자신이 역연산이라 같은 함수를 두 번 부른다.
void splitUpDown(int level)
{
	flipUpDown(N);      // (1) 전체 배열 상하 반전 (위치 + 내부가 함께 뒤집힘)
	flipUpDown(level);  // (2) 각 블록 내부만 다시 뒤집어 복구
}

// 6번: 블록들의 위치만 좌우 반전
void splitLeftRight(int level)
{
	flipLeftRight(N);      // (1) 전체 배열 좌우 반전
	flipLeftRight(level);  // (2) 각 블록 내부 복구
}

// 7번: 블록들의 위치만 시계 방향 회전
//      시계 회전의 역연산은 반시계 회전이다.
void splitClockwise(int level)
{
	clockwise(N);             // (1) 전체 배열 시계 회전
	counterClockwise(level);  // (2) 각 블록 내부를 반시계로 되돌림
}

// 8번: 블록들의 위치만 반시계 방향 회전
void splitCounterClockwise(int level)
{
	counterClockwise(N);  // (1) 전체 배열 반시계 회전
	clockwise(level);     // (2) 각 블록 내부를 시계로 되돌림
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	input();                             // N, R, 배열 입력

	// R개의 연산을 입력받는 즉시 순서대로 수행
	for (int r = 0; r < R; r++)
	{
		int COMMAND, level;              // 연산 번호와 블록 레벨
		scanf("%d %d", &COMMAND, &level);   // 연산 번호와 레벨 입력

		if (COMMAND == FLIP_UPDOWN)                    // 1번
			flipUpDown(level);                         // 블록 내부 상하 반전
		else if (COMMAND == FLIP_LEFTRIGHT)            // 2번
			flipLeftRight(level);                      // 블록 내부 좌우 반전
		else if (COMMAND == CLOCKWISE)                 // 3번
			clockwise(level);                          // 블록 내부 시계 회전
		else if (COMMAND == COUNTER_CLOCKWISE)         // 4번
			counterClockwise(level);                   // 블록 내부 반시계 회전
		else if (COMMAND == SPLIT_UPDOWN)              // 5번
			splitUpDown(level);                        // 블록 위치 상하 반전
		else if (COMMAND == SPLIT_LEFTRIGHT)           // 6번
			splitLeftRight(level);                     // 블록 위치 좌우 반전
		else if (COMMAND == SPLIT_CLOCKWISE)           // 7번
			splitClockwise(level);                     // 블록 위치 시계 회전
		else if (COMMAND == SPLIT_COUNTER_CLOCKWISE)   // 8번
			splitCounterClockwise(level);              // 블록 위치 반시계 회전
	}

	// 최종 결과 출력
	printMap();

	return 0;                            // 프로그램 정상 종료
}
