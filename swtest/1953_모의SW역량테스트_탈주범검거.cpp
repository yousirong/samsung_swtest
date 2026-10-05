/*
	[SWEA] 1953 - [모의 SW 역량테스트] 탈주범 검거
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AV5PpLlKAQ4DFAUq

	■ 문제 요약
	  N x M 지하 터널 지도에 터널 구조물 번호(0 = 없음, 1 ~ 7)가 적혀 있다.
	  탈주범은 맨홀 (R, C)에 들어가 1시간을 보내고, 한 시간에 터널 한 칸씩 이동한다.
	  L시간 뒤 탈주범이 있을 수 있는 칸의 개수를 구한다. 테스트 케이스가 T개.

	    1 : 상하좌우   2 : 상하   3 : 좌우
	    4 : 상우      5 : 하우   6 : 하좌   7 : 상좌

	■ 풀이 방침 : 맨홀에서 BFS, 거리 L 이하인 칸 세기
	  맨홀 칸의 거리를 1(=1시간)로 두고 BFS로 퍼진다.
	  visit[r][c]가 곧 "그 칸에 처음 도착하는 시각"이므로, 마지막에 visit가 1 ~ L인 칸을 센다.

	■ 두 칸이 이어졌는지 = 양쪽 파이프가 서로를 향하는지
	  위로 간다면 "지금 칸에 위 구멍이 있고, 다음 칸에 아래 구멍이 있어야" 한다.
	  한쪽만 뚫려 있으면 못 간다. 이 검사를 isLink(지금, 다음, 방향)가 한다.
	  pipe[번호] = { up, down, right, left } 표를 main()에서 한 번 만들어 둔다.

	■ 주의할 점
	  - 입력 좌표 R, C는 0-based다. 지도는 1-based로 받았으므로 BFS(R + 1, C + 1)로 시작한다.
	  - 테두리(0행, N+1행, 0열, M+1열)를 0(터널 없음)으로 비워 두어 범위 검사를 대신한다.
	  - BFS 안의 cnt는 "마지막으로 넣은 칸의 거리"다. L + 1에 닿으면 더 퍼질 필요가 없어 끝낸다.
	    그때 큐에 남은 칸들은 이미 visit가 매겨져 있으므로 세는 데는 문제가 없다.
	  - isLink의 마지막 return -1은 dir이 1 ~ 4 밖일 때만 오는데, 그런 호출은 없다.
*/
#include <stdio.h>

#define MAX (50+5)

int T;
int N, M, R, C, L;      // 세로, 가로, 맨홀 행, 맨홀 열, 경과 시간
int MAP[MAX][MAX];      // 터널 구조물 번호 (0 = 없음)
int visit[MAX][MAX];    // 처음 도착한 시각 (0 = 못 감)

// 파이프 한 종류가 어느 쪽으로 뚫려 있는지 (1 = 뚫림)
struct PIPE
{
	int up;
	int down;
	int right;
	int left;
};

PIPE pipe[8];

struct RC
{
	int r;
	int c;
};

RC queue[MAX * MAX];
int rp, wp;

/* 0, ↑, →, ↓, ← */
int dr[] = { 0,-1,0,1,0 };
int dc[] = { 0,0,1,0,-1 };

void input()
{
	scanf("%d %d %d %d %d", &N, &M, &R, &C, &L);

	// 테두리 포함 전부 0 -> 테두리는 "터널 없음"이 되어 범위 검사가 필요 없다
	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= M + 1; c++)
			MAP[r][c] = visit[r][c] = 0;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			scanf("%d", &MAP[r][c]);
}

// p1에서 dir 방향으로 p2에 갈 수 있는가 : 서로 마주 보는 구멍이 둘 다 뚫려 있어야 한다
int isLink(PIPE p1, PIPE p2, int dir)
{
	// p1의 위쪽과 p2의 아래쪽 비교
	if (dir == 1) return p1.up && p2.down;
	// p1의 오른쪽과 p2의 왼쪽 비교
	if (dir == 2)return p1.right && p2.left;
	// p1의 아래쪽과 p2의 위쪽 비교
	if (dir == 3)	 return p1.down && p2.up;
	// p1의 왼쪽과 p2의 오른쪽 비교
	if (dir == 4)	return p1.left && p2.right;

	return -1;
}

void BFS(int r, int c)
{
	int cnt;   // 마지막으로 큐에 넣은 칸의 도착 시각

	rp = wp = 0;

	queue[wp].r = r;
	queue[wp++].c = c;
	visit[r][c] = 1;   // 맨홀에 들어간 시각 = 1시간

	cnt = 0;

	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 1; i <= 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 안 가 본 칸 + 터널이 있음 + 두 파이프가 이어짐
			if (visit[nr][nc] == 0 && MAP[nr][nc] != 0
				&& isLink(pipe[MAP[out.r][out.c]], pipe[MAP[nr][nc]], i))
			{
				queue[wp].r = nr;
				queue[wp++].c = nc;
				cnt = visit[nr][nc] = visit[out.r][out.c] + 1;
			}
		}

		// L시간을 넘는 칸이 나오기 시작했으면 더 볼 필요가 없다
		if (cnt == L + 1)return;
	}
}

// 도착 시각이 L 이하인 칸 = 탈주범이 있을 수 있는 칸
int checkVisit()
{
	int sum = 0;
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			if (visit[r][c] != 0 && visit[r][c] <= L) sum++;
	return sum;
}

int main()
{
	// up, down, right, left
	pipe[1] = { 1, 1, 1, 1 };   // 상하좌우
	pipe[2] = { 1, 1, 0, 0 };   // 상하
	pipe[3] = { 0, 0, 1, 1 };   // 좌우
	pipe[4] = { 1, 0, 1, 0 };   // 상우
	pipe[5] = { 0, 1, 1, 0 };   // 하우
	pipe[6] = { 0, 1, 0, 1 };   // 하좌
	pipe[7] = { 1, 0, 0, 1 };   // 상좌

	scanf("%d", &T);

	for (int tc = 1; tc <= T; tc++)
	{
		int ans;

		input();

		BFS(R + 1, C + 1);   // 입력 좌표는 0-based, 지도는 1-based

		ans = checkVisit();

		printf("#%d %d\n", tc, ans);
	}
	return 0;
}