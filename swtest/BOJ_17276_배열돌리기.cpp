// 백준 17276 배열 돌리기
// 45도 단위로 배열을 회전시키는 문제
// 핵심: 전체 배열을 돌리는 것이 아니라
//       "주대각선, 부대각선, 가운데 행, 가운데 열"만 이동시킨다.

#include <stdio.h>

#define MAX (500 + 50)   // N 최대값보다 여유 있게 선언

int T;                   // 테스트 케이스 수
int N, D;                // N: 배열 크기, D: 회전 각도
int MAP[MAX][MAX];       // 원본 배열
int temp[MAX][MAX];      // 회전 전 상태를 잠시 저장할 배열

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &N, &D);

	// N x N 배열 입력
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}
}

// ---------------------------
// 배열 출력 함수
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
// 배열 복사 함수
// original 배열을 copy 배열에 복사
// 회전할 때 덮어쓰기 문제를 막기 위해 사용
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
// 시계 방향으로 45도 회전
// 이동 규칙:
// 가운데 행 -> 주대각선
// 주대각선 -> 가운데 열
// 가운데 열 -> 부대각선
// 부대각선 -> 가운데 행
// ---------------------------
void rotate45Clockwise()
{
	// 현재 MAP 상태를 temp에 복사해 둠
	// 그래야 회전 중 값이 덮어써져도 원본 기준으로 안전하게 이동 가능
	copyMap(temp, MAP);

	int arr[MAX] = { 0 };          // 한 줄(행/열/대각선) 임시 저장용 배열
	int half = (N + 1) / 2;        // 가운데 인덱스

	// 1) 가운데 가로 행 -> 주대각선(왼쪽 위 ~ 오른쪽 아래)
	for (int c = 1; c <= N; c++) arr[c] = temp[half][c];
	for (int i = 1; i <= N; i++) MAP[i][i] = arr[i];

	// 2) 주대각선 -> 가운데 세로 열
	for (int i = 1; i <= N; i++) arr[i] = temp[i][i];
	for (int r = 1; r <= N; r++) MAP[r][half] = arr[r];

	// 3) 가운데 세로 열 -> 부대각선(오른쪽 위 ~ 왼쪽 아래)
	for (int r = 1; r <= N; r++) arr[r] = temp[r][half];
	for (int i = 1; i <= N; i++) MAP[i][N - i + 1] = arr[i];

	// 4) 부대각선 -> 가운데 가로 행
	for (int i = 1; i <= N; i++) arr[i] = temp[N - i + 1][i];
	for (int c = 1; c <= N; c++) MAP[half][c] = arr[c];
}

// ---------------------------
// 반시계 방향으로 45도 회전
// 이동 규칙:
// 가운데 행 -> 부대각선
// 부대각선 -> 가운데 열
// 가운데 열 -> 주대각선
// 주대각선 -> 가운데 행
// ---------------------------
void rotate45CounterClockwise()
{
	// 현재 MAP 상태를 temp에 복사
	copyMap(temp, MAP);

	int arr[MAX] = { 0 };          // 한 줄 임시 저장용
	int half = (N + 1) / 2;        // 가운데 인덱스

	// 1) 가운데 행 -> 부대각선(왼쪽 아래 ~ 오른쪽 위 방향으로 들어감)
	for (int c = 1; c <= N; c++) arr[c] = temp[half][c];
	for (int i = 1; i <= N; i++) MAP[N - i + 1][i] = arr[i];

	// 2) 주대각선 -> 가운데 행
	for (int i = 1; i <= N; i++) arr[i] = temp[i][i];
	for (int c = 1; c <= N; c++) MAP[half][c] = arr[c];

	// 3) 가운데 열 -> 주대각선
	for (int r = 1; r <= N; r++) arr[r] = temp[r][half];
	for (int i = 1; i <= N; i++) MAP[i][i] = arr[i];

	// 4) 부대각선 -> 가운데 열
	for (int i = 1; i <= N; i++) arr[i] = temp[i][N - i + 1];
	for (int r = 1; r <= N; r++) MAP[r][half] = arr[r];
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	scanf("%d", &T);   // 테스트 케이스 수 입력

	for (int tc = 1; tc <= T; tc++)
	{
		input();   // N, D, 배열 입력

		// 45도 회전을 몇 번 해야 하는지 계산
		// 예: 90도면 2번, -135도면 3번
		int count = D / 45;

		// 음수면 절댓값으로 바꿔 반복 횟수만 구함
		count = count > 0 ? count : -count;

		// count번 만큼 45도 회전 수행
		for (int c = 0; c < count; c++)
		{
			if (D >= 0)
				rotate45Clockwise();         // 양수면 시계 방향
			else
				rotate45CounterClockwise();  // 음수면 반시계 방향
		}

		// 회전 후 배열 출력
		printMap();
	}

	return 0;
}