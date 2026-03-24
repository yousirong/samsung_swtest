// 백준 16926, 16927 배열 돌리기 1 / 배열 돌리기 2
// 배열의 각 테두리(layer, ring)를 반시계 방향으로 회전시키는 문제
//
// 핵심 아이디어
// 1. 배열 전체를 한 번에 돌리는 것이 아니라,
//    바깥 테두리 -> 그 안쪽 테두리 -> ... 순서로 각 층을 따로 회전한다.
// 2. 각 테두리의 좌표를 순서대로 arr에 저장한다.
// 3. 회전 횟수만큼 밀린 위치에 값을 넣어 새로운 MAP을 만든다.

#include <stdio.h>

#define MAX (300 + 50)   // N, M 최대보다 여유 있게 선언

int N, M, R;             // N: 행, M: 열, R: 회전 수
int MAP[MAX][MAX];       // 원본 배열
int temp[MAX][MAX];      // 회전 전 상태를 잠시 저장하는 배열

// 좌표를 저장하기 위한 구조체
struct RC
{
	int r;   // row (행)
	int c;   // col (열)
};

typedef struct RC RC;

RC arr[MAX * MAX];
// 현재 테두리의 좌표들을 순서대로 저장할 배열
// 최악의 경우 모든 칸 좌표를 담아도 충분하도록 MAX*MAX 크기로 선언

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &M, &R);

	// N x M 배열 입력
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
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
		for (int c = 1; c <= M; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
}

// ---------------------------
// 배열 복사 함수
// original -> copymap
//
// 회전 중 덮어쓰기 문제를 막기 위해 사용
// 항상 temp에서 읽고 MAP에 쓴다
// ---------------------------
void copyMap(int copymap[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			copymap[r][c] = original[r][c];
		}
	}
}

// ---------------------------
// 하나의 테두리(layer)를 회전시키는 함수
//
// 매개변수 설명
// sr, sc : 시작 좌표 (start row, start col)
// n, m   : 현재 테두리의 세로 길이, 가로 길이
// rotateCount : 회전 횟수
//
// 예를 들어 전체 배열이 4x4라면
// 바깥 테두리는 sr=1, sc=1, n=4, m=4
// 그 안쪽 테두리는 sr=2, sc=2, n=2, m=2
// ---------------------------
void rotatestep(int sr, int sc, int n, int m, int rotateCount)
{
	int er, ec;
	// 끝 좌표 (end row, end col)

	er = sr + n - 1;
	ec = sc + m - 1;

	// 현재 MAP 상태를 temp에 복사
	// 그래야 회전하면서 값을 덮어써도 원래 값 기준으로 처리 가능
	copyMap(temp, MAP);

	int index = 0;   // 현재 테두리 좌표 개수

	// 1) 윗변: (sr, sc) -> (sr, ec)
	for (int c = sc; c <= ec; c++)
	{
		arr[index].r = sr;
		arr[index++].c = c;
	}

	// 2) 오른쪽 변: (sr+1, ec) -> (er, ec)
	// 윗변 마지막 칸은 이미 들어갔으므로 sr+1부터 시작
	for (int r = sr + 1; r <= er; r++)
	{
		arr[index].r = r;
		arr[index++].c = ec;
	}

	// 3) 아랫변: (er, ec-1) -> (er, sc)
	// 오른쪽 아래 모서리는 이미 들어갔으므로 ec-1부터 시작
	for (int c = ec - 1; c >= sc; c--)
	{
		arr[index].r = er;
		arr[index++].c = c;
	}

	// 4) 왼쪽 변: (er-1, sc) -> (sr+1, sc)
	// 아래 왼쪽, 위 왼쪽 모서리는 이미 들어갔으므로 사이만 넣음
	for (int r = er - 1; r >= sr + 1; r--)
	{
		arr[index].r = r;
		arr[index++].c = sc;
	}

	// 이제 arr[0] ~ arr[index-1] 에
	// 현재 테두리의 좌표가 반시계 방향 순서로 저장되어 있음

	// 각 좌표의 값을 회전 후 새로운 위치로 이동
	for (int i = 0; i < index; i++)
	{
		// 현재 i번째 위치의 값이 회전 후 들어갈 위치 계산
		// 반시계 방향 회전이므로 인덱스가 앞으로 당겨짐
		int newIndex = (i - rotateCount) % index;

		// C에서 음수 % 결과 보정
		newIndex = newIndex < 0 ? newIndex + index : newIndex;

		// 회전 후 값이 들어갈 좌표
		RC front = arr[newIndex];

		// arr[i] 위치의 원래 값을 arr[newIndex] 위치에 넣음
		MAP[front.r][front.c] = temp[arr[i].r][arr[i].c];
	}
}

// ---------------------------
// 전체 배열 회전 함수
//
// 배열에는 여러 개의 테두리가 있을 수 있음
// 예)
// 4x4 -> 2개 층
// 5x4 -> 2개 층
// 6x6 -> 3개 층
//
// step = min(N, M) / 2
// ---------------------------
void rotate(int rotateCount)
{
	int sr, sc, n, m;

	// 전체 배열에서 만들 수 있는 테두리 개수
	int step = N < M ? N / 2 : M / 2;

	// 처음엔 가장 바깥 테두리부터 시작
	sr = sc = 1;
	n = N;
	m = M;

	// 각 테두리를 하나씩 회전
	for (int s = 0; s < step; s++)
	{
		rotatestep(sr, sc, n, m, rotateCount);

		// 다음 안쪽 테두리로 이동
		sr++;
		sc++;
		n -= 2;
		m -= 2;
	}
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();     // 입력 받기

	rotate(R);   // R번 회전

	printMap();  // 결과 출력

	return 0;
}