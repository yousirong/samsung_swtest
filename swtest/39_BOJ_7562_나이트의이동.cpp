// BOJ 7562 - 나이트의 이동
// BFS(너비 우선 탐색)로 나이트가 목표 칸까지 가는 최소 이동 횟수를 구하는 문제
//
// 핵심 아이디어
// 1. 나이트는 한 칸씩 움직이는 것이 아니라 8가지 방향으로 점프한다.
// 2. 한 번 움직일 때마다 비용이 모두 1이므로
//    최단 이동 횟수는 BFS로 구할 수 있다.
// 3. MAP 배열을 방문 체크 + 이동 횟수 저장용으로 같이 사용한다.
//
// MAP[r][c]의 의미
// 0 : 아직 방문하지 않은 칸
// 1 : 시작 칸
// 2 : 시작 칸에서 1번 이동해서 도착한 칸
// 3 : 시작 칸에서 2번 이동해서 도착한 칸
// ...
//
// 따라서 목표 지점에 도착했을 때
// 실제 이동 횟수는 MAP[r][c] - 1 이다.

#include <stdio.h>

#define MAX (300 + 30)

int T;                  // 테스트 케이스 수
int L;                  // 체스판 한 변의 길이
int sr, sc;             // 시작 좌표 (start row, start col)
int er, ec;             // 도착 좌표 (end row, end col)
int MAP[MAX][MAX];      // 방문 체크 + 이동 횟수 저장 배열

// 좌표를 저장할 구조체
struct RC
{
	int r;   // row
	int c;   // col
};

typedef struct RC RC;

RC queue[MAX * MAX];    // BFS용 큐

// 나이트가 이동할 수 있는 8가지 방향
int dr[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
int dc[] = { 1,  2, 2, 1, -1, -2, -2, -1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d %d %d", &L, &sr, &sc, &er, &ec);

	// 매 테스트케이스마다 MAP 초기화
	// 0으로 초기화해서 "아직 방문하지 않음" 상태로 만든다
	for (int r = 0; r < L; r++)
	{
		for (int c = 0; c < L; c++)
		{
			MAP[r][c] = 0;
		}
	}
}

// ---------------------------
// 디버그용 출력 함수
// ---------------------------
void printMap()
{
	for (int r = 0; r < L; r++)
	{
		for (int c = 0; c < L; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// BFS 함수
// 시작 좌표 (r, c)에서 목표 좌표 (er, ec)까지의
// 최소 이동 횟수를 반환
// ---------------------------
int BFS(int r, int c)
{
	int rp, wp;
	// rp: read pointer  -> 큐에서 꺼낼 위치
	// wp: write pointer -> 큐에 넣을 위치

	rp = wp = 0;

	// 시작 좌표를 큐에 넣음
	queue[wp].r = r;
	queue[wp++].c = c;

	// 시작 칸 방문 표시
	// 시작 칸의 값을 1로 두고,
	// 이후 이동할 때마다 +1 하면서 거리 정보를 저장
	MAP[r][c] = 1;

	// 큐가 빌 때까지 반복
	while (rp < wp)
	{
		// 현재 위치 하나 꺼냄
		RC out = queue[rp++];

		// 목표 지점에 도착했다면
		// 시작 칸을 1로 뒀으므로 실제 이동 횟수는 -1
		if (out.r == er && out.c == ec)
			return MAP[out.r][out.c] - 1;

		// 나이트의 8가지 이동 방향 탐색
		for (int i = 0; i < 8; i++)
		{
			int nr, nc;   // next row, next col

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 체스판 범위를 벗어나면 건너뜀
			if (nr < 0 || nc < 0 || nr > L - 1 || nc > L - 1)
				continue;

			// 이미 방문한 칸이면 건너뜀
			if (MAP[nr][nc] != 0)
				continue;

			// 방문 가능한 칸이면 큐에 넣음
			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 현재 칸 값 + 1
			// 즉, 시작점으로부터의 이동 횟수를 저장
			MAP[nr][nc] = MAP[out.r][out.c] + 1;
		}
	}

	// 이 문제에서는 보통 도달 가능하지만,
	// 혹시 끝까지 못 찾으면 -1 반환
	return -1;
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	scanf("%d", &T);   // 테스트 케이스 수 입력

	for (int tc = 0; tc < T; tc++)
	{
		input();   // 체스판 크기, 시작점, 도착점 입력

		// 시작점에서 도착점까지 최소 이동 횟수 출력
		printf("%d\n", BFS(sr, sc));
	}

	return 0;
}