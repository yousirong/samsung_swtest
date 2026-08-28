/*
	[BOJ] 2178 - 미로 탐색
	https://www.acmicpc.net/problem/2178

	■ 문제 요약
	  N x M 미로에서 1은 지나갈 수 있는 칸, 0은 벽이다.
	  (1, 1)에서 출발해 (N, M)까지 가는 최단 경로의 칸 수를 출력한다.
	  출발 칸과 도착 칸도 세므로, 바로 옆 칸이면 답은 2다.

	■ 풀이 방침
	  한 칸 이동 비용이 전부 1이므로 BFS가 곧 최단 경로다.
	  BFS는 거리가 가까운 칸부터 차례로 방문하기 때문에,
	  어떤 칸에 "처음 도달했을 때"의 거리가 그 칸까지의 최단 거리다.
	  그래서 이미 방문한 칸은 다시 볼 필요가 없다.

	■ 구현 포인트 : MAP을 거리 배열로 재활용한다
	  별도의 dist 배열을 두지 않고 MAP에 거리를 덮어쓴다.

	    입력 직후 : 1 = 길, 0 = 벽
	    BFS 이후 : MAP[r][c] = (1,1)에서 그 칸까지의 최단 거리

	  이렇게 해도 되는 이유는
	    - 벽은 끝까지 0으로 남아 "MAP == 0" 검사가 계속 유효하고
	    - 길이었던 칸은 방문 순간 거리(2 이상)로 덮이는데,
	      그 칸은 visit 때문에 어차피 다시 검사되지 않기 때문이다.

	  시작 칸 MAP[1][1]은 입력값 1이 그대로 남아 거리 1의 역할을 겸한다.
	  덕분에 "출발 칸도 센다"는 조건이 자연스럽게 맞는다.

	■ 주의할 점
	  경계 검사를 따로 하지 않는데, MAP이 전역 배열이라 바깥쪽이 전부 0(벽)이라서 안전하다.
	  방문 표시는 큐에 "넣을 때" 해야 같은 칸이 중복으로 들어가지 않는다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (100 + 10)

int N, M;                // N: 행 수, M: 열 수

int MAP[MAX][MAX];       // 처음엔 지도(1/0), BFS 이후엔 최단 거리
bool visit[MAX][MAX];    // 방문 여부

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = {  0, 1, 0,-1 };

// 좌표 한 쌍
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX * MAX];     // BFS용 큐 (한 칸이 최대 한 번 들어간다)

// ---------------------------
// 디버그용: 거리와 방문 상태 출력
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
// 입력
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	// 입력이 "101101"처럼 붙어 있으므로 %1d로 한 자리씩 읽는다
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			scanf("%1d", &MAP[r][c]);
		}
	}
}

// ---------------------------
// BFS : (r, c)에서 출발해 도달 가능한 모든 칸의 최단 거리를 MAP에 채운다
// ---------------------------
void BFS(int r, int c)
{
	int rp, wp;   // rp: 꺼낼 위치, wp: 넣을 위치

	rp = wp = 0;

	// 시작 칸을 큐에 넣고 방문 표시
	// (거리는 MAP[r][c]에 남아 있는 입력값 1이 그대로 쓰인다)
	queue[wp].r = r;
	queue[wp++].c = c;

	visit[r][c] = true;

	while (rp < wp)
	{
		RC out = queue[rp++];

		// 상하좌우
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 이동 불가 조건
			//   1) 벽 (MAP == 0). 지도 바깥도 0이라 여기서 함께 걸린다.
			//   2) 이미 방문한 칸 (더 짧은 경로로 이미 도달했다)
			if (MAP[nr][nc] == 0 || visit[nr][nc] == true)
				continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 큐에 넣는 즉시 방문 표시 (중복 삽입 방지)
			visit[nr][nc] = true;

			// 여기서 처음 도달했으므로 이 값이 곧 최단 거리다
			MAP[nr][nc] = MAP[out.r][out.c] + 1;
		}
	}
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	input();

	BFS(1, 1);    // 출발점에서 한 번만 돌리면 모든 칸의 거리가 채워진다

	// 도착점까지의 최단 거리 (문제에서 항상 도달 가능하다고 보장한다)
	printf("%d\n", MAP[N][M]);

	return 0;
}
