// BOJ 2178 - 미로 탐색
// https://www.acmicpc.net/problem/2178
// 문제 요약
// 1은 이동 가능한 칸, 0은 벽이다.
// 시작점 (1,1)에서 도착점 (N,M)까지의 "최단 거리"를 구해야 한다.
//
// 핵심 아이디어
// - 모든 이동 비용이 1이므로 BFS를 사용하면 최단 거리 보장
// - MAP 배열을 "방문 + 거리 저장" 용도로 함께 사용
//
// MAP[r][c]의 의미 변화
// 입력 시:
//   1 -> 이동 가능
//   0 -> 벽
//
// BFS 이후:
//   MAP[r][c] = 시작점에서 해당 칸까지의 최단 거리

#include <stdio.h>
#include <stdbool.h>

#define MAX (100 + 10)

int N, M;                // N: 행, M: 열

int MAP[MAX][MAX];       // 지도 + 거리 저장
bool visit[MAX][MAX];    // 방문 여부 체크

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = {  0, 1, 0,-1 };

// 좌표 구조체
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX * MAX];     // BFS용 큐

// ---------------------------
// 디버그용 출력 함수
// ---------------------------
void printMap()
{
	printf("MAP\n");
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%2d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');

	printf("visit\n");
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%2d ", visit[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	// %1d를 사용하면 "101101" 같은 문자열을 한 자리씩 읽어온다
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			scanf("%1d", &MAP[r][c]);
		}
	}
}

// ---------------------------
// BFS 함수
// 시작점 (r, c)에서 모든 칸까지의 최단 거리 계산
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

	// BFS 시작
	while (rp < wp)
	{
		// 현재 위치 꺼냄
		RC out = queue[rp++];

		// 4방향 탐색
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// ---------------------------
			// 이동 불가능 조건
			// 1) 벽인 경우 (MAP == 0)
			// 2) 이미 방문한 경우
			// ---------------------------
			if (MAP[nr][nc] == 0 || visit[nr][nc] == true)
				continue;

			// 큐에 추가
			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 방문 처리
			visit[nr][nc] = true;

			// ---------------------------
			// 거리 갱신
			// 현재 칸까지 거리 + 1
			// ---------------------------
			MAP[nr][nc] = MAP[out.r][out.c] + 1;
		}
	}
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();      // 입력 받기

	BFS(1, 1);    // 시작점에서 BFS

	// 도착점까지의 최단 거리 출력
	printf("%d\n", MAP[N][M]);

	return 0;
}