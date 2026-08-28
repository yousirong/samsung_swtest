/*
	[BOJ] 2206 벽 부수고 이동하기 / 14442 벽 부수고 이동하기 2
	https://www.acmicpc.net/problem/2206
	https://www.acmicpc.net/problem/14442

	■ 이 파일에 대하여
	  두 문제의 풀이가 위아래로 들어 있는 정리용 파일이다.
	  main과 전역 변수가 각각 있어 한꺼번에 컴파일되지 않으니,
	  돌려 볼 때는 쓰지 않을 쪽을 통째로 주석 처리한다.

	■ 문제 요약
	  N x M 지도에서 0은 빈칸, 1은 벽이다. (1,1)에서 (N,M)까지 최단 거리로 이동한다.
	  이동 중에 벽을 부술 수 있는데, 부술 수 있는 횟수가 문제마다 다르다.

	    2206  : 최대 1번
	    14442 : 최대 K번 (K <= 10)

	  갈 수 없으면 -1을 출력한다. 거리는 시작 칸과 도착 칸을 모두 포함해서 센다.

	■ 핵심 아이디어 : 상태를 (좌표 + 부순 횟수)로 확장한다
	  36번(미로 탐색)처럼 visit[r][c] 하나만 쓰면 틀린다.
	  같은 칸이라도 "벽을 아직 안 부수고 도착한 나"와 "이미 부수고 도착한 나"는
	  앞으로 할 수 있는 일이 다르기 때문이다.
	  먼저 도착했다는 이유로 뒤에 오는 쪽을 막아 버리면 정답 경로를 놓친다.

	  그래서 방문 배열에 차원을 하나 더 붙인다.

	    visit[crash][r][c] = 벽을 crash번 부순 상태로 (r, c)에 도달한 최단 거리
	                         (0이면 아직 그 상태로 와 본 적 없음)

	  이렇게 두면 지도가 "층이 여러 개인 입체 미로"가 된다.
	    같은 층 이동 : 빈칸으로 갈 때 (crash 유지)
	    다음 층 이동 : 벽을 부수고 갈 때 (crash + 1)
	  벽을 부수는 이동은 항상 위층으로만 가고 되돌아올 수 없으므로 사이클이 생기지 않는다.

	  상태 수가 (K+1) * N * M 이고 각 상태는 큐에 한 번만 들어가므로,
	  일반 BFS와 똑같이 "처음 도달한 거리 = 최단 거리"가 그대로 성립한다.

	■ 구현 포인트
	  1) visit이 방문 여부와 거리를 겸한다. 그래서 시작점을 0이 아니라 1로 놓는다.
	     (0을 "미방문"으로 쓰기 때문에 거리를 1부터 시작해야 구분된다.
	      마침 문제도 시작 칸을 거리에 포함하므로 계산이 맞아떨어진다)
	  2) 도착 판정은 큐에서 꺼낼 때 한다. BFS는 거리가 작은 상태부터 꺼내므로
	     처음 꺼낸 도착 상태가 곧 최단이다.
	  3) 경계 검사는 명시적으로 한다. 36번처럼 바깥이 0이면 "빈칸"으로 읽혀
	     지도 밖으로 걸어 나가 버리기 때문에, 여기서는 생략할 수 없다.
*/

// ====================================================================
// BOJ 2206 - 벽 부수고 이동하기 (벽을 최대 1번)
//
//   crash = 0 : 아직 벽을 부수지 않은 상태
//   crash = 1 : 이미 한 번 부순 상태 (이제 벽은 통과 불가)
// ====================================================================

#include <stdio.h>

#define MAX (1000 + 10)

int N, M;                       // N: 행 수, M: 열 수
int MAP[MAX][MAX];              // 지도 (0: 빈칸, 1: 벽)
int visit[2][MAX][MAX];         // visit[부순 횟수][r][c] = 그 상태로 도달한 거리 (0이면 미방문)

// 큐에 담을 "상태"
struct RC
{
	int r;       // 행
	int c;       // 열
	int crash;   // 지금까지 벽을 부순 횟수 (0 또는 1)
};

typedef struct RC RC;

// 상태 수가 좌표 수의 2배이므로 큐도 2배로 잡는다
RC queue[MAX * MAX * 2];

// 4방향: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	// "0101"처럼 붙어 있는 입력이라 %1d로 한 자리씩 읽는다
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			scanf("%1d", &MAP[r][c]);
}

// ---------------------------
// 디버그용 출력
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

// 지도와 두 층의 거리 배열을 한꺼번에 출력
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
// BFS
//   시작점 (r, c)에서 (N, M)까지의 최단 거리. 못 가면 -1.
// ---------------------------
int BFS(int r, int c)
{
	int rp, wp;   // rp: 꺼낼 위치, wp: 넣을 위치

	rp = wp = 0;

	// 시작은 벽을 한 번도 안 부순 상태. 거리는 1부터 (0은 미방문을 뜻하므로)
	visit[0][r][c] = 1;

	queue[wp].r = r;
	queue[wp].c = c;
	queue[wp++].crash = 0;

	while (rp < wp)
	{
		RC out = queue[rp++];

		// 처음 꺼낸 도착 상태가 최단이다
		if (out.r == N && out.c == M)
			return visit[out.crash][out.r][out.c];

		for (int i = 0; i < 4; i++)
		{
			int nr = out.r + dr[i];
			int nc = out.c + dc[i];

			// 지도 바깥은 명시적으로 걸러야 한다 (바깥값 0이 빈칸으로 오해되기 때문)
			if (nr < 1 || nc < 1 || nr > N || nc > M)
				continue;

			// ---------------------------
			// 아직 벽을 부수지 않은 상태 : 두 가지 선택지가 있다
			// ---------------------------
			if (out.crash == 0)
			{
				// (a) 빈칸으로 그냥 이동 -> 같은 층(crash = 0)에 머문다
				if (MAP[nr][nc] == 0 && visit[0][nr][nc] == 0)
				{
					queue[wp].r = nr;
					queue[wp].c = nc;
					queue[wp++].crash = 0;

					visit[0][nr][nc] = visit[0][out.r][out.c] + 1;
				}
				else
				{
					// (b) 벽을 부수고 이동 -> 위층(crash = 1)으로 넘어간다
					//     단, 그 층에서 아직 방문한 적 없는 칸이어야 한다
					if (MAP[nr][nc] == 1 && visit[1][nr][nc] == 0)
					{
						queue[wp].r = nr;
						queue[wp].c = nc;
						queue[wp++].crash = 1;

						// 거리는 "현재 층의 거리 + 1"에서 이어진다
						visit[1][nr][nc] = visit[0][out.r][out.c] + 1;
					}
				}
			}
			// ---------------------------
			// 이미 한 번 부순 상태 : 빈칸으로만 갈 수 있다
			// ---------------------------
			else
			{
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

	// 큐가 다 비도록 도착하지 못했다
	return -1;
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	input();

	printf("%d\n", BFS(1, 1));

	// printMapAll(); // 디버그용

	return 0;
}


// ====================================================================
// BOJ 14442 - 벽 부수고 이동하기 2 (벽을 최대 K번, K <= 10)
//
//   2206을 그대로 일반화한 것이다. 층이 2개에서 K+1개로 늘어났을 뿐,
//   "빈칸이면 같은 층 / 벽이면 한 층 위" 라는 규칙은 똑같다.
//
//   달라지는 곳은 세 군데뿐이다.
//     visit 배열의 첫 차원 : 2 -> K+1 (여유 있게 11)
//     분기 조건           : crash == 0 -> crash < K
//     다음 상태           : crash 고정 -> out.crash / out.crash + 1
// ====================================================================

#include <stdio.h>

#define MAX (1000 + 10)

int N, M, K;                    // N: 행 수, M: 열 수, K: 부술 수 있는 벽의 최대 개수
int MAP[MAX][MAX];              // 지도 (0: 빈칸, 1: 벽)
int visit[11][MAX][MAX];        // visit[부순 횟수][r][c] = 그 상태로 도달한 거리

// 큐에 담을 "상태"
struct RC
{
	int r;       // 행
	int c;       // 열
	int crash;   // 지금까지 부순 벽 개수 (0 ~ K)
};

typedef struct RC RC;

// 상태 수가 (K+1) * N * M 이고 각 상태는 큐에 한 번만 들어가므로 이 크기면 충분하다.
// (다만 메모리를 꽤 먹는 편이라, 빠듯하면 좌표를 정수 하나로 인코딩해 줄일 수 있다)
RC queue[MAX * MAX * 11];

// 4방향: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &M, &K);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			scanf("%1d", &MAP[r][c]);
}

// ---------------------------
// 디버그용 출력
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

// 앞쪽 두 층만 확인용으로 출력
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
// BFS
//   시작점 (r, c)에서 (N, M)까지의 최단 거리. 못 가면 -1.
// ---------------------------
int BFS(int r, int c)
{
	int rp, wp;

	rp = wp = 0;

	// 시작은 0번 부순 상태, 거리 1
	visit[0][r][c] = 1;

	queue[wp].r = r;
	queue[wp].c = c;
	queue[wp++].crash = 0;

	while (rp < wp)
	{
		RC out = queue[rp++];

		// 처음 꺼낸 도착 상태가 최단
		if (out.r == N && out.c == M)
			return visit[out.crash][out.r][out.c];

		for (int i = 0; i < 4; i++)
		{
			int nr = out.r + dr[i];
			int nc = out.c + dc[i];

			// 지도 바깥 제외
			if (nr < 1 || nc < 1 || nr > N || nc > M)
				continue;

			// ---------------------------
			// 아직 부술 여유가 남은 경우 (crash < K)
			// ---------------------------
			if (out.crash < K)
			{
				// (a) 빈칸이면 같은 층 유지
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
					// (b) 벽이면 하나 부수고 한 층 위로
					if (MAP[nr][nc] == 1 && visit[out.crash + 1][nr][nc] == 0)
					{
						queue[wp].r = nr;
						queue[wp].c = nc;
						queue[wp++].crash = out.crash + 1;

						// 거리는 현재 층의 거리에서 이어 붙인다
						visit[out.crash + 1][nr][nc] =
							visit[out.crash][out.r][out.c] + 1;
					}
				}
			}
			// ---------------------------
			// K번을 다 쓴 경우 : 빈칸으로만 이동
			// ---------------------------
			else
			{
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

	return -1;
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	input();

	printf("%d\n", BFS(1, 1));

	// printMapAll(); // 디버그용

	return 0;
}
