/*
	[BOJ] 2667 - 단지번호붙이기
	https://www.acmicpc.net/problem/2667

	■ 문제 요약
	  N x N 지도에 집이 있는 칸은 1, 없는 칸은 0으로 표시되어 있다.
	  상하좌우로 붙어 있는 집들을 하나의 "단지"로 볼 때,
	  단지의 총 개수와 각 단지에 속한 집의 수를 오름차순으로 출력한다.

	■ 풀이 방침
	  전형적인 연결 요소(connected component) 세기 문제다.

	    지도를 왼쪽 위부터 훑는다
	      -> 아직 방문하지 않은 집을 만나면 그 칸에서 BFS를 시작한다
	      -> BFS는 연결된 집을 전부 방문 처리하고 그 개수를 돌려준다
	      -> BFS를 시작한 횟수가 단지 수, 돌려받은 값이 각 단지의 크기

	  이미 방문한 칸에서는 BFS를 시작하지 않으므로 한 단지는 정확히 한 번만 세어진다.

	■ 구현 포인트
	  1) BFS가 끝났을 때 wp(큐에 넣은 총 개수)가 곧 그 단지의 집 수다.
	     방문 표시를 "큐에 넣을 때" 하기 때문에 한 칸이 큐에 두 번 들어가지 않고,
	     따라서 별도의 카운터 없이 wp를 그대로 쓸 수 있다.
	  2) 지도 바깥을 검사하는 코드가 없는데도 안전한 이유는
	     MAP이 전역 배열이라 0으로 초기화되어 있고, 인덱스 0과 N+1 줄이 전부 0(빈칸)이기 때문이다.
	     그래서 경계를 넘어가면 "집이 아님"으로 자동으로 걸러진다.
	  3) 입력이 "0110100"처럼 공백 없이 붙어 있으므로 scanf("%1d", ...)로 한 자리씩 읽는다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (25 + 5)   // 문제의 최대 N은 25, 경계 여유를 위해 +5

int N;                 // 지도의 크기 (N x N)

int MAP[MAX][MAX];     // 지도 (1: 집, 0: 빈칸)
bool visit[MAX][MAX];  // 방문 여부

// 각 단지의 집 개수. 모든 집이 따로 떨어져 있으면 단지가 N*N개까지 나올 수 있다.
int answers[MAX * MAX];

// 좌표 한 쌍
struct RC
{
	int r;   // row (행)
	int c;   // col (열)
};

typedef struct RC RC;

// BFS용 큐. 한 칸이 최대 한 번만 들어가므로 N*N이면 충분하다.
RC queue[MAX * MAX];

// 방향 배열: 상, 우, 하, 좌 (이 문제는 4방향만 연결로 본다)
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 디버그용: visit 배열 출력
// ---------------------------
void printVisit()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", visit[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d", &N);

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			// 입력이 "0110100"처럼 공백 없이 붙어 있다.
			// %1d는 딱 한 자리만 읽어 주므로 이런 형식에 그대로 쓸 수 있다.
			scanf("%1d", &MAP[r][c]);
		}
	}
}

// ---------------------------
// BFS : (r, c)가 속한 단지를 전부 방문 처리하고 그 크기를 반환
// ---------------------------
int BFS(int r, int c)
{
	int rp, wp;   // rp: 꺼낼 위치, wp: 넣을 위치

	rp = wp = 0;

	// 시작 칸을 큐에 넣고 바로 방문 표시
	queue[wp].r = r;
	queue[wp++].c = c;

	visit[r][c] = true;

	// 처리할 칸이 남아 있는 동안
	while (rp < wp)
	{
		RC out = queue[rp++];

		// 상하좌우 네 방향
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 집이 아니거나 이미 방문했으면 건너뛴다.
			// 지도 바깥은 MAP이 0이라 이 조건에서 함께 걸러진다(경계 검사 불필요).
			if (MAP[nr][nc] == 0 || visit[nr][nc] == true)
				continue;

			// 같은 단지에 속한 집이므로 큐에 넣는다
			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 넣는 즉시 방문 표시 (중복 삽입 방지 = wp가 곧 정확한 개수)
			visit[nr][nc] = true;
		}
	}

	// 큐에 들어간 칸의 총 개수 = 이 단지의 집 수
	return wp;
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	input();

	int ansCount = 0;   // 단지 수

	// 지도를 왼쪽 위부터 훑으면서 새 단지의 시작점을 찾는다
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			// 집인데 아직 방문 전이라면, 여기가 새로운 단지의 시작점이다
			if (MAP[r][c] == 1 && visit[r][c] == false)
			{
				// BFS가 단지 전체를 방문 처리하고 크기를 돌려준다
				answers[ansCount++] = BFS(r, c);
			}
		}
	}

	// 1) 총 단지 수
	printf("%d\n", ansCount);

	// 2) 단지 크기를 오름차순 정렬 (선택 정렬)
	for (int i = 0; i < ansCount - 1; i++)
	{
		for (int k = i + 1; k < ansCount; k++)
		{
			if (answers[i] > answers[k])
			{
				int tmp = answers[i];
				answers[i] = answers[k];
				answers[k] = tmp;
			}
		}
	}

	// 3) 각 단지의 크기
	for (int i = 0; i < ansCount; i++)
		printf("%d\n", answers[i]);

	return 0;
}
