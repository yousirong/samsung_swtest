/*
	[BOJ] 2178 - 미로 탐색 (연습 버전)
	https://www.acmicpc.net/problem/2178

	■ 요약
	  (1, 1)에서 (N, M)까지의 최단 경로 칸 수를 BFS로 구한다.
	  MAP을 거리 배열로 재활용한다 (자세한 설명은 36_BOJ_2178_미로탐색.cpp 참고).

	■ 주의 (연습 코드의 버그)
	  BFS의 while문 안에서 큐를 꺼낼 때 queue[wp++]로 되어 있다.
	  꺼내는 쪽은 rp이므로 queue[rp++]여야 한다.
	  지금 코드는 rp가 영원히 0이고 wp만 커져서 while문이 끝나지 않는다.
*/
#include <stdio.h>	              // scanf, printf, putchar 사용

#define MAX (100+10)              // N, M 최대 100 + 여유

int N, M;                         // N: 행 수, M: 열 수

int MAP[MAX][MAX];                // 처음엔 지도(1/0), BFS 이후엔 최단 거리
bool visit[MAX][MAX];             // 방문 여부

// 상, 우, 하, 좌
int dr[] = { -1,0,1,0 };          // 방향별 행 변화량
int dc[] = { 0,1,0,-1 };          // 방향별 열 변화량

struct RC                         // 좌표 한 쌍
{
	int r;                        // 행
	int c;                        // 열
};

RC queue[MAX * MAX];              // BFS용 큐

void printMap() // for debug
{
	printf("MAP\n");                     // 거리 배열 제목
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= M; c++)     // 모든 열
			printf("%2d", MAP[r][c]);    // 거리 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄

	printf("visit\n");                   // 방문 배열 제목
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= M; c++)     // 모든 열
			printf("%d ", visit[r][c]);  // 방문 여부 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

void input()
{
	scanf("%d %d", &N, &M);              // 행 수와 열 수 입력

	for (int r = 1; r <= N; r++)         // 1행부터 N행까지
		for (int c = 1; c <= M; c++)     // 1열부터 M열까지
			scanf("%1d", &MAP[r][c]);    // 붙어 있는 숫자를 한 자리씩 입력
}

void BFS(int r, int c)
{
	int rp, wp;                          // rp: 꺼낼 위치, wp: 넣을 위치

	rp = wp = 0;                         // 빈 큐

	queue[wp].r = r;                     // 시작 행
	queue[wp++].c = c;                   // 시작 열, 삽입 위치 증가

	visit[r][c] = true;                  // 시작 칸 방문 표시

	//printMap();

	while (rp < wp)                      // 처리할 칸이 남아 있는 동안
	{
		RC out = queue[wp++];            // ※ 버그: 꺼낼 때는 queue[rp++]여야 한다 (지금은 무한 루프)

		for (int i = 0; i < 4; i++)      // 상하좌우 네 방향
		{
			int nr, nc;                  // 이웃 칸 좌표

			nr = out.r + dr[i];          // 이웃 행
			nc = out.c + dc[i];          // 이웃 열

			if (MAP[nr][nc] == 0 || visit[nr][nc] == true) continue;   // 벽(바깥 포함)이거나 방문했으면 건너뜀

			queue[wp].r = nr;            // 이웃 행 기록
			queue[wp++].c = nc;          // 이웃 열 기록, 삽입 위치 증가

			visit[nr][nc] = true;        // 넣는 즉시 방문 표시

			MAP[nr][nc] = MAP[out.r][out.c] + 1;   // 처음 도달한 거리 = 최단 거리
		}
	}
}

int main()
{
	input();                             // 미로 입력

	BFS(1, 1);                           // (1, 1)에서 BFS 시작

	printf("%d\n", MAP[N][M]);           // 도착점까지의 최단 거리 출력

	return 0;                            // 프로그램 정상 종료
}
