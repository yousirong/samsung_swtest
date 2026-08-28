/*
	[BOJ] 4963 - 섬의 개수
	https://www.acmicpc.net/problem/4963

	■ 문제 요약
	  W x H 크기의 지도에서 1은 땅, 0은 바다다.
	  가로/세로뿐 아니라 대각선으로도 이어져 있으면 같은 섬으로 본다(8방향 연결).
	  지도에 있는 섬의 개수를 출력한다.
	  테스트 케이스가 여러 개 이어지고, "0 0"이 입력되면 끝난다.

	■ 풀이 방침
	  34번(단지번호붙이기)과 완전히 같은 연결 요소 세기 문제이고, 다른 점은 둘뿐이다.

	    1) 방향이 4개가 아니라 8개 (대각선 포함)
	    2) 크기를 물어보지 않으므로 BFS는 방문 처리만 하고 개수는 반환하지 않는다

	  지도를 훑다가 방문하지 않은 땅을 만나면 BFS로 그 섬 전체를 지워 나가고,
	  BFS를 시작한 횟수가 곧 섬의 개수다.

	■ 주의할 점 (여러 테스트 케이스)
	  케이스가 반복되므로 MAP과 visit을 매번 초기화해야 한다.
	  이때 실제 영역(1 ~ H, 1 ~ W)뿐 아니라 바깥 한 줄(0행/0열, H+1행/W+1열)까지 0으로 채운다.
	  그래야 이전 케이스가 더 컸을 때 남아 있던 값이 경계 밖에서 "땅"으로 읽히는 사고를 막고,
	  BFS에서 좌표 범위 검사를 생략할 수 있다.

	  또 하나, 종료 판정("0 0")을 하려면 W와 H를 먼저 읽어야 하므로
	  input()을 호출한 다음에 break를 검사하는 순서로 되어 있다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (50 + 5)

int W, H;               // W: 가로(열 개수), H: 세로(행 개수)

int MAP[MAX][MAX];      // 지도 (1: 땅, 0: 바다)
bool visit[MAX][MAX];   // 방문 여부

// 좌표 한 쌍
struct RC
{
	int r;   // row (행)
	int c;   // col (열)
};

typedef struct RC RC;

RC queue[MAX * MAX];    // BFS용 큐 (한 칸이 최대 한 번 들어간다)

// 8방향 : ↑, ↗, →, ↘, ↓, ↙, ←, ↖
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
// 입력 (테스트 케이스 하나)
// ---------------------------
void input()
{
	scanf("%d %d", &W, &H);

	// 테스트 케이스마다 초기화.
	// 바깥 한 줄까지 함께 0으로 밀어 두면 경계 밖이 항상 바다로 취급되어
	// BFS에서 좌표 범위 검사를 하지 않아도 된다.
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
// BFS : (r, c)가 속한 섬 전체를 방문 처리
// ---------------------------
void BFS(int r, int c)
{
	int rp, wp;   // rp: 꺼낼 위치, wp: 넣을 위치

	rp = wp = 0;

	// 시작 칸을 큐에 넣고 바로 방문 표시
	queue[wp].r = r;
	queue[wp++].c = c;

	visit[r][c] = true;

	while (rp < wp)
	{
		RC out = queue[rp++];

		// 대각선을 포함한 8방향
		for (int i = 0; i < 8; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 바다이거나 이미 방문한 칸이면 건너뛴다 (경계 밖도 0이라 여기서 걸린다)
			if (MAP[nr][nc] == 0 || visit[nr][nc] == true)
				continue;

			// 같은 섬이므로 큐에 넣는다
			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 넣는 즉시 방문 표시 (중복 삽입 방지)
			visit[nr][nc] = true;
		}
	}
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	// 테스트 케이스를 계속 처리
	while (1)
	{
		input();

		// 종료 조건은 W, H를 읽어 봐야 알 수 있으므로 입력 뒤에 검사한다
		if (W == 0 && H == 0)
			break;

		int ansCount = 0;   // 섬의 개수

		// 지도를 훑으며 아직 방문하지 않은 땅 = 새로운 섬의 시작점을 찾는다
		for (int r = 1; r <= H; r++)
		{
			for (int c = 1; c <= W; c++)
			{
				if (MAP[r][c] == 1 && visit[r][c] == false)
				{
					BFS(r, c);   // 이 섬 전체를 방문 처리
					ansCount++;  // BFS를 시작한 횟수 = 섬 개수
				}
			}
		}

		printf("%d\n", ansCount);
	}

	return 0;
}
