/*
	[BOJ] 17276 - 배열 돌리기

	[프로그래머스 제출용]  원본 : swtest/12_BOJ_17276_배열돌리기.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).

	https://www.acmicpc.net/problem/17276

	■ 문제 요약
	  홀수 크기 N x N 배열과 각도 D(-360 ~ 360, 45의 배수)가 주어진다.
	  배열을 D도만큼 회전시킨 결과를 출력한다. D가 양수면 시계 방향, 음수면 반시계 방향.

	  단, 여기서 말하는 "회전"은 배열 전체가 도는 것이 아니라
	  아래 네 줄에 놓인 값들만 서로 자리를 옮기는 것이다.

	    - 주대각선   : (1,1) (2,2) ... (N,N)
	    - 부대각선   : (1,N) (2,N-1) ... (N,1)
	    - 가운데 가로 행 : (half, 1) ~ (half, N)
	    - 가운데 세로 열 : (1, half) ~ (N, half)      // half = (N+1)/2

	  네 줄에 속하지 않는 칸은 값이 그대로 유지된다.

	■ 풀이 방침
	  45도 회전 1번을 함수로 만들고, |D| / 45 번 반복한다.
	  45도 시계 회전에서 네 줄은 아래 순서로 한 칸씩 밀린다.

	    가운데 행 → 주대각선 → 가운데 열 → 부대각선 → 가운데 행 (원형)

	  반시계는 그 반대 방향이다.

	■ 핵심 규칙 (좌표 대응)
	  한 줄이 다음 줄로 옮겨갈 때, 행 좌표나 열 좌표 중 "하나는 그대로 보존"된다.
	    가운데 행 (half, c)  → 주대각선 (c, c)        : 열 c 보존
	    주대각선 (i, i)      → 가운데 열 (i, half)    : 행 i 보존
	    가운데 열 (r, half)  → 부대각선 (r, N-r+1)    : 행 r 보존
	    부대각선 (N-i+1, i)  → 가운데 행 (half, i)    : 열 i 보존
	  이 대응만 정확히 잡으면 방향 헷갈릴 일이 없다.

	■ 주의할 점
	  1) 네 줄을 순서대로 옮기면 앞 단계가 쓴 값을 뒷 단계가 읽어버린다.
	     그래서 회전 시작 전에 MAP을 temp로 통째로 복사해 두고,
	     "읽기는 항상 temp에서, 쓰기는 항상 MAP에" 라는 규칙을 지킨다.
	  2) 중심 칸 (half, half)는 네 줄 모두에 동시에 속하지만
	     어느 경로로 계산해도 temp[half][half]가 들어오므로 회전해도 값이 변하지 않는다.
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

using namespace std;

#define MAX (500 + 50)   // N 최대값보다 여유 있게 선언

int T;                   // 테스트 케이스 수
int N, D;                // N: 배열 크기(홀수), D: 회전 각도(45의 배수)
int MAP[MAX][MAX];       // 원본 배열, 1-based
int temp[MAX][MAX];      // 회전 직전 상태 스냅샷(읽기 전용으로 사용)

// ---------------------------
// 입력
// ---------------------------
// [수정] scanf 대신 인자로 받는다
void input(int d, const vector<vector<int>>& board)
{
	N = (int)board.size();   // [수정] scanf("%d %d", &N, &D) 대체
	D = d;

	// N x N 배열 입력
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			MAP[r][c] = board[r - 1][c - 1];   // [수정] scanf 대체 (0-based -> 1-based)
		}
	}
}

// ---------------------------
// 결과 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
}

// ---------------------------
// original -> copy 로 배열 전체 복사
//
// 회전 도중 "이미 새 값이 덮인 칸"을 원본인 줄 알고 읽는 사고를 막기 위한 스냅샷.
// ---------------------------
void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			copy[r][c] = original[r][c];
		}
	}
}

// ---------------------------
// 시계 방향 45도 회전 1회
//
// 가운데 행 → 주대각선 → 가운데 열 → 부대각선 → 가운데 행
// ---------------------------
void rotate45Clockwise()
{
	// 회전 전 상태를 통째로 저장. 아래 네 단계는 전부 temp에서만 읽는다.
	copyMap(temp, MAP);

	int arr[MAX] = { 0 };          // 한 줄(행/열/대각선)을 잠시 담아 두는 버퍼
	int half = (N + 1) / 2;        // 가운데 인덱스 (N이 홀수라 딱 떨어진다)

	// 1) 가운데 가로 행 -> 주대각선   (half, c) -> (c, c), 열 좌표 보존
	for (int c = 1; c <= N; c++) arr[c] = temp[half][c];
	for (int i = 1; i <= N; i++) MAP[i][i] = arr[i];

	// 2) 주대각선 -> 가운데 세로 열   (i, i) -> (i, half), 행 좌표 보존
	for (int i = 1; i <= N; i++) arr[i] = temp[i][i];
	for (int r = 1; r <= N; r++) MAP[r][half] = arr[r];

	// 3) 가운데 세로 열 -> 부대각선   (r, half) -> (r, N-r+1), 행 좌표 보존
	for (int r = 1; r <= N; r++) arr[r] = temp[r][half];
	for (int i = 1; i <= N; i++) MAP[i][N - i + 1] = arr[i];

	// 4) 부대각선 -> 가운데 가로 행   (N-i+1, i) -> (half, i), 열 좌표 보존
	for (int i = 1; i <= N; i++) arr[i] = temp[N - i + 1][i];
	for (int c = 1; c <= N; c++) MAP[half][c] = arr[c];
}

// ---------------------------
// 반시계 방향 45도 회전 1회
//
// 시계 회전의 정확히 역방향
// 가운데 행 → 부대각선 → 가운데 열 → 주대각선 → 가운데 행
// ---------------------------
void rotate45CounterClockwise()
{
	// 마찬가지로 회전 전 스냅샷을 떠 둔다.
	copyMap(temp, MAP);

	int arr[MAX] = { 0 };          // 한 줄 임시 버퍼
	int half = (N + 1) / 2;        // 가운데 인덱스

	// 1) 가운데 행 -> 부대각선       (half, c) -> (N-c+1, c), 열 좌표 보존
	for (int c = 1; c <= N; c++) arr[c] = temp[half][c];
	for (int i = 1; i <= N; i++) MAP[N - i + 1][i] = arr[i];

	// 2) 주대각선 -> 가운데 행       (i, i) -> (half, i), 열 좌표 보존
	for (int i = 1; i <= N; i++) arr[i] = temp[i][i];
	for (int c = 1; c <= N; c++) MAP[half][c] = arr[c];

	// 3) 가운데 열 -> 주대각선       (r, half) -> (r, r), 행 좌표 보존
	for (int r = 1; r <= N; r++) arr[r] = temp[r][half];
	for (int i = 1; i <= N; i++) MAP[i][i] = arr[i];

	// 4) 부대각선 -> 가운데 열       (i, N-i+1) -> (i, half), 행 좌표 보존
	for (int i = 1; i <= N; i++) arr[i] = temp[i][N - i + 1];
	for (int r = 1; r <= N; r++) MAP[r][half] = arr[r];
}

// ---------------------------
// 메인
// ---------------------------
// [수정] main() -> solution(). 원본 main의 회전 횟수 계산을 그대로 옮겼다.
vector<vector<int>> solution(int d, vector<vector<int>> board)
{
	input(d, board);   // N, D, 배열 세팅

	// 45도 단위가 몇 번인지 계산 (예: 90도 -> 2번, -135도 -> 3번)
	int count = D / 45;

	// 반복 횟수는 절댓값만 필요하다. 방향은 D의 부호로 따로 판단.
	count = count > 0 ? count : -count;

	// count번 만큼 45도 회전 반복 (D가 0이면 한 번도 돌지 않는다)
	for (int c = 0; c < count; c++)
	{
		if (D >= 0)
			rotate45Clockwise();         // 양수 -> 시계 방향
		else
			rotate45CounterClockwise();  // 음수 -> 반시계 방향
	}

	// [수정] printMap() -> 격자를 그대로 반환
	vector<vector<int>> answer(N, vector<int>(N));
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			answer[r - 1][c - 1] = MAP[r][c];

	return answer;
}
