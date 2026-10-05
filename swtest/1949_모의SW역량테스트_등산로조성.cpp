/*
	[SWEA] 1949 - [모의 SW 역량테스트] 등산로 조성
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AV5PoOKKAPIDFAUq

	■ 문제 요약
	  N x N 지도에 봉우리 높이가 적혀 있다 (3 <= N <= 8, 높이 1 ~ 20).
	  등산로는 "가장 높은 봉우리"에서 시작해 상하좌우로 "더 낮은 칸"으로만 이어진다.
	  딱 한 칸을 골라 최대 K만큼 깎을 수 있다 (1 <= K <= 5, 안 깎아도 된다).
	  만들 수 있는 가장 긴 등산로의 길이(칸 수)를 구한다. 테스트 케이스가 T개.

	■ 풀이 방침 : 가장 높은 봉우리마다 DFS
	  1) 입력을 받으며 최댓값을 구하고, 최댓값인 칸을 전부 start[]에 모은다.
	  2) 각 시작점에서 DFS(길이, 현재 칸, 깎았는지 flag)로 갈 수 있는 끝까지 간다.
	       - 다음 칸이 더 낮으면 그냥 간다
	       - 더 낮지 않아도, 아직 안 깎았고 깎아서(K 이하) 낮출 수 있으면
	         "현재 칸 - 1" 높이로 깎고 간다. 돌아오면 원래 높이로 되돌린다.
	  3) DFS에 들어올 때마다 길이 L로 최댓값을 갱신한다.

	■ 깎을 때는 "현재 높이 - 1"까지만 깎는다
	  더 깊이 깎을수록 그 칸이 낮아져 다음으로 갈 수 있는 칸이 줄어든다.
	  그래서 "지나갈 수 있는 가장 높은 높이" = 현재 - 1 로 깎는 것이 항상 가장 유리하다.
	  깎는 양은 next - (cur - 1) 이고, 이것이 K 이하인 조건이 cur > next - K 다.

	■ 주의할 점
	  - 테두리를 -1로 막아 범위 검사를 대신한다. 높이는 1 이상이고,
	    깎아도 최소 0까지만 내려가므로 -1과 겹치지 않는다.
	  - 등산로는 높이가 엄격히 줄어들기 때문에 지나온 칸으로 되돌아갈 수가 없다.
	    visit 검사는 사실상 걸리지 않지만 안전장치로 남아 있다.
	  - flag는 매개변수(지역 변수)라서 1로 바꿨다가 0으로 되돌리는 것이 자기 호출에만 영향을 준다.
*/
#include <stdio.h>

#define MAX (10+5)

int T;

int N, K;               // 지도 크기, 최대 공사 깊이
int MAP[MAX][MAX];      // 높이. 테두리는 -1
int visit[MAX][MAX];    // 지금 등산로에 포함된 칸

struct RC
{
	int r;
	int c;
};

RC start[MAX * MAX];    // 가장 높은 봉우리 좌표들
int scnt;

// 왼쪽, 위, 오른쪽, 아래
int dr[] = { 0,-1,0,1 };
int dc[] = { -1,0,1,0 };

int MAXANS;             // 가장 긴 등산로 길이

void input()
{
	int max;

	scanf("%d %d", &N, &K);

	// 테두리까지 -1로 채워 두고 안쪽만 입력으로 덮는다
	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= N + 1; c++)
			MAP[r][c] = -1;

	max = 0;
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%d", &MAP[r][c]);
			if (max < MAP[r][c])max = MAP[r][c];
		}
	}

	// 가장 높은 봉우리를 전부 시작점으로 모은다
	scnt = 0;
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == max)
			{
				start[scnt].r = r;
				start[scnt++].c = c;
			}
		}
	}
}

// L : 지금까지 등산로 길이, (sr, sc) : 현재 칸, flag : 이미 한 번 깎았으면 1
void DFS(int L, int sr,int sc, int flag)
{
	if (MAXANS < L) MAXANS = L;

	for (int dir = 0; dir < 4; dir++)
	{
		int nr, nc;

		nr = sr + dr[dir];
		nc = sc + dc[dir];

		if (MAP[nr][nc] == -1) continue;                 // 지도 밖
		if (MAP[sr][sc] <= MAP[nr][nc] - K) continue;    // K만큼 깎아도 현재보다 낮아지지 않는다

		// 다음칸이 더 작은 경우
		if (MAP[sr][sc] > MAP[nr][nc] && visit[nr][nc] == 0)
		{
			visit[sr][sc] = 1;
			DFS(L + 1, nr, nc, flag);
			visit[sr][sc] = 0;
		}
		// 다음칸을 깎아서 더 작은 경우
		// (바로 위 continue를 통과했으므로 MAP[sr][sc] > MAP[nr][nc] - K 는 이미 참이다)
		else if (MAP[sr][sc] > MAP[nr][nc] - K && flag == 0 && visit[nr][nc] == 0)
		{
			int tmp = MAP[nr][nc];   // 되돌릴 원래 높이

			visit[sr][sc] = 1;
			flag = 1;
			// 2칸이상 (sr, sc) 보다 작을 필요가 없다.
			MAP[nr][nc] = MAP[sr][sc] - 1;

			DFS(L + 1, nr, nc, flag);

			// 원상복구 : 높이, 깎은 표시, 방문 표시
			MAP[nr][nc] = tmp;
			flag = 0;
			visit[sr][sc] = 0;
		}
	}
}

int main()
{
	scanf("%d", &T);
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		MAXANS = 0;
		for (int i = 0; i < scnt; i++)
			DFS(1, start[i].r, start[i].c, 0);   // 시작 칸 자체가 길이 1
		printf("#%d %d\n", tc, MAXANS);
	}
}