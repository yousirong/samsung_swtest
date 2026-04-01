// BOJ 2206 - 벽 부수고 이동하기
// BFS + 상태 확장
// https://www.acmicpc.net/problem/2206
// 핵심 아이디어:
// 같은 좌표라도 "벽을 부순 적이 있는지"에 따라 다른 상태로 봐야 한다.
// 그래서 visit[r][c] 가 아니라
// visit[crash][r][c] 형태로 관리한다.
//
// crash = 0 : 아직 벽을 부수지 않은 상태
// crash = 1 : 이미 벽을 한 번 부순 상태

#include <stdio.h>

#define MAX (1000 + 10)

int N, M;                       // N: 행, M: 열
int MAP[MAX][MAX];              // 지도 정보 (0: 빈칸, 1: 벽)
int visit[2][MAX][MAX];         // visit[벽 부순 횟수][r][c] = 해당 상태로 도착한 거리

// BFS 큐에 넣을 상태
struct RC
{
	int r;       // 현재 행
	int c;       // 현재 열
	int crash;   // 지금까지 벽을 부순 횟수 (0 또는 1)
};

typedef struct RC RC;

RC queue[MAX * MAX * 2];        // 상태 수가 2배이므로 큐도 2배 크기

// 4방향: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	// %1d 를 사용하면 0101 같이 붙어 있는 숫자를 한 자리씩 읽을 수 있음
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			scanf("%1d", &MAP[r][c]);
}

// ---------------------------
// 디버그용 출력 함수
// ---------------------------
void printMap(int map[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 디버그용: 전체 상태 출력
// ---------------------------
void printMapAll()
{
	printf("MAP\n");
	printMap(MAP);

	printf("visit[0]\n");
	printMap(visit[0]);

	printf("visit[1]\n");
	printMap(visit[1]);
}

// ---------------------------
// BFS 함수
// 시작점: (r, c)
// 반환값:
//   목적지까지의 최단 거리
//   불가능하면 -1
// ---------------------------
int BFS(int r, int c)
{
	int rp, wp;   // rp: read pointer, wp: write pointer

	rp = wp = 0;

	// 시작점은 벽을 아직 안 부순 상태(crash=0)로 시작
	visit[0][r][c] = 1;

	queue[wp].r = r;
	queue[wp].c = c;
	queue[wp++].crash = 0;

	while (rp < wp)
	{
		// 큐에서 현재 상태 꺼내기
		RC out = queue[rp++];

		// 목적지 도착 시 현재 상태의 거리 반환
		if (out.r == N && out.c == M)
			return visit[out.crash][out.r][out.c];

		// 4방향 탐색
		for (int i = 0; i < 4; i++)
		{
			int nr = out.r + dr[i];
			int nc = out.c + dc[i];

			// 범위를 벗어나면 무시
			if (nr < 1 || nc < 1 || nr > N || nc > M)
				continue;

			// ---------------------------
			// 아직 벽을 한 번도 안 부순 상태
			// ---------------------------
			if (out.crash == 0)
			{
				// 다음 칸이 빈칸이고,
				// 아직 "벽을 안 부순 상태"로 방문하지 않았다면 이동 가능
				if (MAP[nr][nc] == 0 && visit[0][nr][nc] == 0)
				{
					queue[wp].r = nr;
					queue[wp].c = nc;
					queue[wp++].crash = 0;

					visit[0][nr][nc] = visit[0][out.r][out.c] + 1;
				}
				else
				{
					// 다음 칸이 벽이라면, 이번에 벽을 부수고 이동 가능
					// 단, 아직 "벽 1번 부순 상태"로 방문한 적이 없어야 함
					if (MAP[nr][nc] == 1 && visit[1][nr][nc] == 0)
					{
						queue[wp].r = nr;
						queue[wp].c = nc;
						queue[wp++].crash = 1;

						visit[1][nr][nc] = visit[0][out.r][out.c] + 1;
					}
				}
			}
			// ---------------------------
			// 이미 벽을 한 번 부순 상태
			// 이제는 벽을 더 부술 수 없음
			// ---------------------------
			else
			{
				// 다음 칸이 빈칸이고,
				// 아직 "벽 1번 부순 상태"로 방문하지 않았다면 이동 가능
				if (MAP[nr][nc] == 0 && visit[1][nr][nc] == 0)
				{
					queue[wp].r = nr;
					queue[wp].c = nc;
					queue[wp++].crash = 1;

					visit[1][nr][nc] = visit[1][out.r][out.c] + 1;
				}
			}
		}
	}

	// 끝까지 도달하지 못한 경우
	return -1;
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();

	printf("%d\n", BFS(1, 1));

	// printMapAll(); // 디버그용

	return 0;
}


// BOJ 14442 - 벽 부수고 이동하기 2
// https://www.acmicpc.net/problem/14442
// BFS + 상태 확장
//
// 핵심 아이디어:
// 같은 좌표라도 "지금까지 벽을 몇 번 부쉈는지"가 다르면
// 서로 다른 상태로 봐야 한다.
//
// visit[crash][r][c]
// = 벽을 crash번 부순 상태로 (r, c)에 도착한 최단 거리
//
//crash는 0 ~ K 까지 가능

#include <stdio.h>

#define MAX (1000 + 10)

int N, M, K;                    // N: 행, M: 열, K: 부술 수 있는 벽의 최대 개수
int MAP[MAX][MAX];              // 지도 정보 (0: 빈칸, 1: 벽)
int visit[11][MAX][MAX];        // visit[부순 벽 개수][r][c]

// BFS 큐에 넣을 상태
struct RC
{
	int r;       // 현재 행
	int c;       // 현재 열
	int crash;   // 지금까지 부순 벽 개수
};

typedef struct RC RC;

RC queue[MAX * MAX * 11];       // 상태 수가 최대 11배이므로 큐도 충분히 크게

// 4방향: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &M, &K);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			scanf("%1d", &MAP[r][c]);
}

// ---------------------------
// 디버그용 출력 함수
// ---------------------------
void printMap(int map[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 디버그용: 전체 상태 일부 출력
// ---------------------------
void printMapAll()
{
	printf("MAP\n");
	printMap(MAP);

	printf("visit[0]\n");
	printMap(visit[0]);

	printf("visit[1]\n");
	printMap(visit[1]);
}

// ---------------------------
// BFS 함수
// 시작점: (r, c)
// 반환값:
//   목적지까지의 최단 거리
//   불가능하면 -1
// ---------------------------
int BFS(int r, int c)
{
	int rp, wp;

	rp = wp = 0;

	// 시작점은 벽을 0번 부순 상태
	visit[0][r][c] = 1;

	queue[wp].r = r;
	queue[wp].c = c;
	queue[wp++].crash = 0;

	while (rp < wp)
	{
		// 현재 상태 꺼내기
		RC out = queue[rp++];

		// 목적지 도착 시 해당 상태의 거리 반환
		if (out.r == N && out.c == M)
			return visit[out.crash][out.r][out.c];

		// 4방향 탐색
		for (int i = 0; i < 4; i++)
		{
			int nr = out.r + dr[i];
			int nc = out.c + dc[i];

			// 범위를 벗어나면 무시
			if (nr < 1 || nc < 1 || nr > N || nc > M)
				continue;

			// ---------------------------
			// 아직 벽을 더 부술 수 있는 경우
			// ---------------------------
			if (out.crash < K)
			{
				// 다음 칸이 빈칸이면 현재 crash 상태 유지
				if (MAP[nr][nc] == 0 && visit[out.crash][nr][nc] == 0)
				{
					queue[wp].r = nr;
					queue[wp].c = nc;
					queue[wp++].crash = out.crash;

					visit[out.crash][nr][nc] =
						visit[out.crash][out.r][out.c] + 1;
				}
				else
				{
					// 다음 칸이 벽이면 하나 부수고 이동 가능
					// 단, crash+1 상태로 아직 방문 안 했어야 함
					if (MAP[nr][nc] == 1 && visit[out.crash + 1][nr][nc] == 0)
					{
						queue[wp].r = nr;
						queue[wp].c = nc;
						queue[wp++].crash = out.crash + 1;

						visit[out.crash + 1][nr][nc] =
							visit[out.crash][out.r][out.c] + 1;
					}
				}
			}
			// ---------------------------
			// 이미 K번 다 부순 경우
			// 이제는 벽을 더 부술 수 없음
			// ---------------------------
			else
			{
				// 빈칸으로만 이동 가능
				if (MAP[nr][nc] == 0 && visit[out.crash][nr][nc] == 0)
				{
					queue[wp].r = nr;
					queue[wp].c = nc;
					queue[wp++].crash = out.crash;

					visit[out.crash][nr][nc] =
						visit[out.crash][out.r][out.c] + 1;
				}
			}
		}
	}

	// 목적지까지 갈 수 없는 경우
	return -1;
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();

	printf("%d\n", BFS(1, 1));

	// printMapAll(); // 디버그용

	return 0;
}