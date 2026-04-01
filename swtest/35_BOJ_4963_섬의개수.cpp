// BOJ 4963 - 섬의 개수
// https://www.acmicpc.net/problem/4963
// 문제 요약
// - 1은 땅, 0은 바다
// - 상하좌우뿐 아니라 대각선까지 포함한 8방향으로 연결되면
//   같은 섬으로 본다.
// - 지도에 있는 섬의 개수를 구하는 문제
//
// 핵심 아이디어
// - 아직 방문하지 않은 땅(1)을 만나면 BFS 시작
// - 그 땅과 연결된 모든 땅을 한 번에 방문 처리
// - BFS를 시작한 횟수가 곧 섬의 개수

#include <stdio.h>
#include <stdbool.h>

#define MAX (50 + 5)

int W, H;               // W: 가로(열 개수), H: 세로(행 개수)

int MAP[MAX][MAX];      // 지도 정보 (1: 땅, 0: 바다)
bool visit[MAX][MAX];   // 방문 여부 체크 배열

// 좌표를 저장하기 위한 구조체
struct RC
{
	int r;   // row(행)
	int c;   // col(열)
};

typedef struct RC RC;

RC queue[MAX * MAX];    // BFS용 큐

// 8방향 탐색
// ↑, ↗, →, ↘, ↓, ↙, ←, ↖
int dr[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
int dc[] = {  0,  1, 1, 1, 0,-1,-1, -1 };

// ---------------------------
// 디버그용: visit 배열 출력
// ---------------------------
void printVisit()
{
	for (int r = 1; r <= H; r++)
	{
		for (int c = 1; c <= W; c++)
			printf("%d ", visit[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &W, &H);

	// 매 테스트케이스마다 배열 초기화
	// 바깥 한 줄까지 0으로 채워 두면
	// 경계 밖을 바다처럼 취급할 수 있다.
	for (int r = 0; r <= H + 1; r++)
	{
		for (int c = 0; c <= W + 1; c++)
		{
			MAP[r][c] = 0;
			visit[r][c] = false;
		}
	}

	// 실제 지도 입력
	for (int r = 1; r <= H; r++)
	{
		for (int c = 1; c <= W; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}
}

// ---------------------------
// BFS 함수
// 시작 좌표 (r, c)가 속한 섬 전체를 방문 처리
// ---------------------------
void BFS(int r, int c)
{
	int rp, wp;
	// rp: read pointer  -> 큐에서 꺼낼 위치
	// wp: write pointer -> 큐에 넣을 위치

	rp = wp = 0;

	// 시작 좌표를 큐에 넣음
	queue[wp].r = r;
	queue[wp++].c = c;

	// 시작 좌표 방문 처리
	visit[r][c] = true;

	// 큐가 빌 때까지 반복
	while (rp < wp)
	{
		// 현재 좌표 하나 꺼냄
		RC out = queue[rp++];

		// 8방향 탐색
		for (int i = 0; i < 8; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 바다이거나 이미 방문한 칸이면 건너뜀
			if (MAP[nr][nc] == 0 || visit[nr][nc] == true)
				continue;

			// 연결된 땅이면 큐에 추가
			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 방문 처리
			visit[nr][nc] = true;
		}
	}
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	// 여러 테스트케이스를 반복 처리
	while (1)
	{
		input();

		// 0 0 이 입력되면 종료
		if (W == 0 && H == 0)
			break;

		int ansCount = 0;   // 섬의 개수

		// 지도 전체를 돌면서
		// 아직 방문하지 않은 땅을 만나면 새로운 섬 시작
		for (int r = 1; r <= H; r++)
		{
			for (int c = 1; c <= W; c++)
			{
				if (MAP[r][c] == 1 && visit[r][c] == false)
				{
					BFS(r, c);   // 해당 섬 전체 방문 처리
					ansCount++;  // BFS 시작 횟수 = 섬 개수
				}
			}
		}

		// 현재 테스트케이스의 섬 개수 출력
		printf("%d\n", ansCount);
	}

	return 0;
}