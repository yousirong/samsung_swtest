/*
	[BOJ] 2194 - 유닛 이동시키기
	https://www.acmicpc.net/problem/2194

	■ 문제 요약
	  N x M 격자에 장애물 K개가 놓여 있다.
	  A x B 크기의 직사각형 유닛을 상하좌우로 한 칸씩 움직여
	  시작 위치에서 도착 위치까지 옮기는 최소 이동 횟수를 구한다.
	  도달할 수 없으면 -1을 출력한다.
	  시작/도착 좌표는 모두 유닛의 "왼쪽 위 칸" 기준으로 주어진다.

	■ 풀이 방침
	  유닛이 여러 칸을 차지하지만, 크기와 모양이 절대 변하지 않으므로
	  상태를 "왼쪽 위 좌표 (r, c)" 하나로 압축할 수 있다.
	  그러면 평범한 격자 BFS와 똑같은 문제가 된다.

	  다만 한 칸으로 볼 수 없는 부분이 하나 있다. 이동 가능 판정이다.
	  (nr, nc)로 옮기려면 두 가지를 모두 만족해야 한다.

	    1) 유닛 전체가 격자 안에 들어와야 한다
	       왼쪽 위가 (nr, nc)면 오른쪽 아래는 (nr + A - 1, nc + B - 1)이므로
	       이 끝점이 N, M을 넘지 않아야 한다.
	    2) 유닛이 덮게 될 A x B 칸에 장애물이 하나도 없어야 한다.

	  나머지는 36번(미로 탐색)과 같다. 이동 비용이 전부 1이라 BFS가 곧 최단이다.

	■ 구현 포인트
	  visit이 방문 여부와 거리를 겸한다. 0을 미방문으로 쓰기 때문에
	  시작점을 1로 두고, 답을 낼 때 1을 빼 준다.

	■ 성능에 대해
	  isEmpty()가 이동 한 번마다 A x B 칸을 전부 다시 훑는다.
	  유닛이 크면 이 부분이 병목이 되는데, 두 가지로 줄일 수 있다.
	    - 한 칸 움직이면 새로 덮이는 칸은 앞쪽 한 줄(A칸 또는 B칸)뿐이므로 그 줄만 검사한다
	    - 장애물 개수의 2차원 누적합을 미리 만들어 두면 직사각형 판정이 O(1)이 된다
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (500 + 50)

int N, M;              // 격자 크기 (행, 열)
int A, B;              // 유닛의 세로 크기 A, 가로 크기 B
int K;                 // 장애물 개수
int sr, sc, er, ec;    // 시작/도착 좌표 (유닛의 왼쪽 위 기준)

int MAP[MAX][MAX];     // 1: 장애물, 0: 빈칸
int visit[MAX][MAX];   // 방문 여부 + 이동 횟수(1부터) 저장

// 좌표 한 쌍
struct RC
{
	int r;   // row
	int c;   // col
};

typedef struct RC RC;

RC queue[MAX * MAX];   // BFS용 큐 (한 상태가 최대 한 번 들어간다)

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = {  0, 1, 0,-1 };

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d %d %d %d", &N, &M, &A, &B, &K);

	// 장애물 위치
	for (int k = 0; k < K; k++)
	{
		int r, c;

		scanf("%d %d", &r, &c);

		MAP[r][c] = 1;
	}

	// 시작/도착 좌표 (유닛의 왼쪽 위 칸)
	scanf("%d %d %d %d", &sr, &sc, &er, &ec);
}

// ---------------------------
// 디버그용 맵 출력
// ---------------------------
void printMap(int map[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 왼쪽 위가 (sr, sc)일 때 유닛이 덮는 A x B 칸에 장애물이 없는지
//
// 하나라도 장애물이 있으면 false.
// (격자 범위 검사는 호출하기 전에 이미 끝냈다고 가정한다)
// ---------------------------
bool isEmpty(int sr, int sc)
{
	for (int r = sr; r <= sr + A - 1; r++)
	{
		for (int c = sc; c <= sc + B - 1; c++)
		{
			if (MAP[r][c] == 1)
				return false;
		}
	}

	return true;
}

// ---------------------------
// BFS : 왼쪽 위 좌표 (r, c)에서 (er, ec)까지의 최소 이동 횟수
// ---------------------------
int BFS(int r, int c)
{
	int rp, wp;   // rp: 꺼낼 위치, wp: 넣을 위치

	rp = wp = 0;

	// 시작 상태. 0이 미방문이므로 거리를 1부터 센다.
	visit[r][c] = 1;

	queue[wp].r = r;
	queue[wp++].c = c;

	while (rp < wp)
	{
		RC out = queue[rp++];

		// 처음 꺼낸 도착 상태가 최단이다 (1부터 셌으므로 -1)
		if (out.r == er && out.c == ec)
			return visit[out.r][out.c] - 1;

		// 상, 우, 하, 좌
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			// 옮긴 뒤의 왼쪽 위 좌표
			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// (1) 유닛 전체가 격자 안에 들어와야 한다.
			//     왼쪽 위가 (nr, nc)면 오른쪽 아래는 (nr + A - 1, nc + B - 1).
			if (nr < 1 || nc < 1 || (nr + A - 1) > N || (nc + B - 1) > M)
				continue;

			// (2) 유닛이 덮을 영역에 장애물이 없어야 하고
			// (3) 아직 방문하지 않은 상태여야 한다
			if (isEmpty(nr, nc) == false || visit[nr][nc] != 0)
				continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 값을 넣는 순간이 방문 표시이자 거리 기록
			visit[nr][nc] = visit[out.r][out.c] + 1;
		}
	}

	// 큐가 다 비도록 도착하지 못했다
	return -1;
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	input();

	printf("%d\n", BFS(sr, sc));

	return 0;
}
