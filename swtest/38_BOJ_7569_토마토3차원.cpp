/*
	[BOJ] 7569 - 토마토 (3차원)
	https://www.acmicpc.net/problem/7569

	■ 문제 요약
	  M x N 크기의 상자를 H층으로 쌓아 놓은 3차원 창고다.

	     1 : 이미 익은 토마토
	     0 : 아직 익지 않은 토마토
	    -1 : 토마토가 들어 있지 않은 빈 칸

	  하루가 지나면 익은 토마토와 "면이 맞닿은" 6방향(같은 층 상하좌우 + 위층 + 아래층)의
	  안 익은 토마토가 익는다. 모두 익는 최소 날짜를 출력하고,
	  처음부터 다 익어 있으면 0, 끝내 다 익지 못하면 -1을 출력한다.

	■ 풀이 방침
	  2차원 버전(7576)과 완전히 같은 멀티 소스 BFS이고, 이동 방향이 4개에서 6개로 늘었을 뿐이다.

	    - 처음부터 익은 토마토를 전부 큐에 넣고 한 번만 BFS를 돌린다
	    - MAP 자체를 "며칠째에 익었는가" 기록판으로 쓴다 (처음 익은 것 = 1)
	    - 값을 넣는 순간이 곧 방문 표시라서 visit 배열이 따로 필요 없다
	    - 정답은 (최댓값 - 1), 0이 남아 있으면 -1

	  이 코드는 같은 층 4방향만 dr/dc 배열로 돌리고,
	  위/아래층 이동은 배열 없이 아래에 두 덩이로 따로 적었다.
	  (dh/dr/dc를 6칸짜리 배열로 합쳐 한 루프로 처리해도 결과는 같다)

	■ 주의할 점
	  1) 입력 순서가 M(가로) N(세로) H(높이) 순이다. 헷갈리기 쉬운 부분.
	  2) 테두리는 반드시 -1로 채운다. 0으로 두면 창고 밖이 "안 익은 토마토"로 읽혀
	     BFS가 밖으로 새어 나가고, 정답 계산에서도 영원히 안 익은 칸으로 잡힌다.
	     h 방향으로도 0층과 H+1층을 -1로 밀어 둔 덕분에 층 경계 검사도 필요 없다.
*/

#include <stdio.h>

#define MAX (100 + 10)

int M, N, H;                 // M: 가로(열), N: 세로(행), H: 층 수
int MAP[MAX][MAX][MAX];      // [층][행][열] 상태 겸 "익은 날짜" 기록판

// 3차원 좌표
struct HRC
{
	int h;   // height (층)
	int r;   // row    (행)
	int c;   // col    (열)
};

typedef struct HRC HRC;

HRC queue[MAX * MAX * MAX];  // BFS용 큐 (한 칸이 최대 한 번 들어간다)

// 같은 층 안에서의 4방향: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d %d", &M, &N, &H);

	// 6면 전부(위/아래층 포함) 바깥 테두리를 -1로 채운다.
	// 그래야 BFS와 정답 계산 어느 쪽에서도 경계 검사를 하지 않아도 된다.
	for (int h = 0; h <= H + 1; h++)
	{
		for (int r = 0; r <= N + 1; r++)
		{
			for (int c = 0; c <= M + 1; c++)
			{
				MAP[h][r][c] = -1;
			}
		}
	}

	// 실제 상자 입력 (아래층부터 한 층씩)
	for (int h = 1; h <= H; h++)
	{
		for (int r = 1; r <= N; r++)
		{
			for (int c = 1; c <= M; c++)
			{
				scanf("%d", &MAP[h][r][c]);
			}
		}
	}
}

// ---------------------------
// 디버그용 출력 (테두리까지 함께 확인)
// ---------------------------
void printMap()
{
	for (int h = 0; h <= H + 1; h++)
	{
		printf("h : %d\n", h);
		for (int r = 0; r <= N + 1; r++)
		{
			for (int c = 0; c <= M + 1; c++)
				printf("%d ", MAP[h][r][c]);
			putchar('\n');
		}
		putchar('\n');
	}
}

// ---------------------------
// 멀티 소스 BFS
//
// 처음부터 익은 토마토를 모두 시작점으로 넣고 6방향으로 퍼뜨린다.
// ---------------------------
void BFS()
{
	int rp, wp;   // rp: 꺼낼 위치, wp: 넣을 위치

	rp = wp = 0;

	// 시작점 모으기 : 값이 1인 칸을 전부 큐에 넣는다 (여러 곳에서 동시에 출발)
	for (int h = 1; h <= H; h++)
	{
		for (int r = 1; r <= N; r++)
		{
			for (int c = 1; c <= M; c++)
			{
				if (MAP[h][r][c] == 1)
				{
					queue[wp].h = h;
					queue[wp].r = r;
					queue[wp++].c = c;
				}
			}
		}
	}

	while (rp < wp)
	{
		HRC out = queue[rp++];

		// ---------------------------
		// (1) 같은 층에서 상하좌우 4방향
		// ---------------------------
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 0이 아니면 건너뛴다 (-1은 빈 칸/바깥, 1 이상은 이미 익음 = 방문 완료)
			if (MAP[out.h][nr][nc] != 0)
				continue;

			queue[wp].h = out.h;
			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 값을 넣는 순간이 방문 표시. 현재 칸 날짜 + 1이 이웃의 날짜다.
			MAP[out.h][nr][nc] = MAP[out.h][out.r][out.c] + 1;
		}

		// ---------------------------
		// (2) 바로 아래층 (같은 행/열)
		// ---------------------------
		if (MAP[out.h - 1][out.r][out.c] == 0)
		{
			queue[wp].h = out.h - 1;
			queue[wp].r = out.r;
			queue[wp++].c = out.c;

			MAP[out.h - 1][out.r][out.c] = MAP[out.h][out.r][out.c] + 1;
		}

		// ---------------------------
		// (3) 바로 위층 (같은 행/열)
		// ---------------------------
		if (MAP[out.h + 1][out.r][out.c] == 0)
		{
			queue[wp].h = out.h + 1;
			queue[wp].r = out.r;
			queue[wp++].c = out.c;

			MAP[out.h + 1][out.r][out.c] = MAP[out.h][out.r][out.c] + 1;
		}
	}
}

// ---------------------------
// 정답 계산
// ---------------------------
int getAnswer()
{
	int maxAnswer = 0;

	for (int h = 1; h <= H; h++)
	{
		for (int r = 1; r <= N; r++)
		{
			for (int c = 1; c <= M; c++)
			{
				// 0이 남아 있다 = BFS가 닿지 못한 토마토가 있다
				if (MAP[h][r][c] == 0)
					return -1;

				// 가장 늦게 익은 칸의 값
				if (maxAnswer < MAP[h][r][c])
					maxAnswer = MAP[h][r][c];
			}
		}
	}

	// 날짜를 1부터 셌으므로 경과 일수는 최댓값 - 1
	// (처음부터 전부 익어 있으면 최댓값 1 -> 0)
	return maxAnswer - 1;
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	input();

	BFS();

	printf("%d\n", getAnswer());

	return 0;
}
