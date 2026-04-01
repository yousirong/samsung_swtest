// BOJ 7569 - 토마토
// https://www.acmicpc.net/problem/7569
// 3차원 BFS(너비 우선 탐색)으로 모든 토마토가 익는 최소 날짜를 구하는 문제
//
// 값의 의미
//  1 : 처음부터 익은 토마토
//  0 : 아직 익지 않은 토마토
// -1 : 토마토가 없는 칸
//
// 2차원 토마토(7576)와 거의 같지만,
// 이번에는 위층 / 아래층 이동이 추가된 3차원 BFS 문제이다.
//
// 핵심 아이디어
// 1. 처음부터 익어 있는 모든 토마토를 큐에 한 번에 넣고 시작한다.
// 2. BFS를 돌리면서 인접한 안 익은 토마토를 하루 뒤에 익게 만든다.
// 3. MAP 배열 자체를 "익은 날짜"처럼 사용한다.
//    예: 처음 익은 토마토 = 1, 다음 날 익은 토마토 = 2, ...
// 4. BFS가 끝난 뒤
//    - 아직 0이 남아 있으면 모두 익지 못한 것 -> -1
//    - 아니면 최대값 - 1 이 정답

#include <stdio.h>

#define MAX (100 + 10)

int M, N, H;                 // M: 가로(열), N: 세로(행), H: 높이(층 수)
int MAP[MAX][MAX][MAX];      // 3차원 토마토 상자 상태

// 3차원 좌표를 저장할 구조체
struct HRC
{
	int h;   // height (층)
	int r;   // row    (행)
	int c;   // col    (열)
};

typedef struct HRC HRC;

HRC queue[MAX * MAX * MAX];  // BFS용 큐

// 같은 층 안에서의 4방향 이동: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d", &M, &N, &H);

	// 바깥 테두리를 전부 -1로 초기화
	// 범위를 벗어난 곳도 "없는 칸"처럼 처리하기 위함
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

	// 실제 상자 입력
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
// 디버그용 출력 함수
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
// BFS 함수
//
// 처음부터 익은 모든 토마토를 큐에 넣고 시작하는
// "멀티 소스 BFS"
// ---------------------------
void BFS()
{
	int rp, wp;
	// rp: read pointer  -> 큐에서 꺼낼 위치
	// wp: write pointer -> 큐에 넣을 위치

	rp = wp = 0;

	// 처음부터 익어 있는 모든 토마토(값이 1)를 큐에 넣는다.
	// 여러 시작점에서 동시에 퍼져나가는 효과
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

	// 큐가 빌 때까지 BFS 진행
	while (rp < wp)
	{
		// 현재 익은 토마토 하나 꺼냄
		HRC out = queue[rp++];

		// ---------------------------
		// 같은 층에서 4방향 탐색
		// ---------------------------
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 아직 안 익은 토마토(0)만 전파 가능
			if (MAP[out.h][nr][nc] != 0)
				continue;

			// 큐에 넣고
			queue[wp].h = out.h;
			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 현재 칸 값 + 1
			// 즉, 며칠째에 익었는지 기록
			MAP[out.h][nr][nc] = MAP[out.h][out.r][out.c] + 1;
		}

		// ---------------------------
		// 바로 아래층으로 전파
		// ---------------------------
		if (MAP[out.h - 1][out.r][out.c] == 0)
		{
			queue[wp].h = out.h - 1;
			queue[wp].r = out.r;
			queue[wp++].c = out.c;

			MAP[out.h - 1][out.r][out.c] = MAP[out.h][out.r][out.c] + 1;
		}

		// ---------------------------
		// 바로 위층으로 전파
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
// 정답 계산 함수
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
				// 아직 0이 남아 있으면
				// 끝까지 익지 못한 토마토가 있다는 뜻
				if (MAP[h][r][c] == 0)
					return -1;

				// 가장 큰 값을 찾음
				// 마지막으로 익은 날짜 정보
				if (maxAnswer < MAP[h][r][c])
					maxAnswer = MAP[h][r][c];
			}
		}
	}

	// 처음 익은 토마토가 1부터 시작하므로
	// 실제 날짜는 최대값 - 1
	return maxAnswer - 1;
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();   // 입력 받기

	BFS();     // 3차원 BFS 수행

	printf("%d\n", getAnswer());  // 결과 출력

	return 0;
}