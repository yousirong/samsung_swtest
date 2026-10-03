/*
	[코드트리] 2025 하반기 오후 1번 - AI 로봇청소기
	https://www.codetree.ai/ko/frequent-problems/samsung-sw/problems/ai-robot/description

	■ 문제 요약
	  N x N 격자의 각 칸에는 먼지 양이 적혀 있고, -1은 물건(지나갈 수 없음)이다.
	  로봇청소기 K대가 있고, L번 테스트한다. 한 번의 테스트는 아래 순서다.

	    1) 이동 : 1번 청소기부터 차례로, 가장 가까운 "먼지 있는 칸"으로 간다.
	       물건과 다른 청소기는 지나갈 수 없다. 거리가 같으면 행이 작은 칸, 열이 작은 칸.
	       지금 칸에 이미 먼지가 있으면 움직이지 않는다. 갈 곳이 없으면 제자리에 있는다.
	    2) 청소 : 1번 청소기부터, 자기 칸과 상하좌우 중 세 방향(한 방향은 뺀다)을 청소한다.
	       칸마다 최대 20까지 먼지를 지운다.
	       어느 방향을 뺄지는 "청소할 수 있는 먼지 합"이 가장 큰 쪽으로 정한다
	       (같으면 오른쪽 -> 아래 -> 왼쪽 -> 위 순).
	    3) 축적 : 먼지가 있는 칸마다 5씩 늘어난다.
	    4) 확산 : 먼지가 0인 칸은 상하좌우 먼지 합의 1/10(몫)만큼 먼지가 생긴다(동시에).
	    5) 출력 : 격자 전체 먼지 합을 출력한다.

	■ 풀이 방침
	  - 테두리를 WALL(-1)로 두어 격자 밖과 물건을 같은 값으로 처리한다.
	  - 이동은 BFS로 가장 가까운 먼지 칸을 찾는다. 같은 거리면 행 -> 열이 작은 칸을 고른다.
	    다른 청소기 위치는 check[][]에 찍어 두고 통과하지 못하게 한다.
	  - 청소 방향은 "다섯 칸(자기 + 상하좌우) 합에서 빠지는 한 칸을 뺀 값"이 가장 큰 쪽이다.
	    total에 다섯 칸의 min(먼지, 20)을 모두 더해 두고, 방향마다 반대쪽 한 칸을 뺀 값을 비교한다.
	  - 확산은 "동시에"이므로 tmpMAP에 모았다가 한 번에 더한다.

	■ 주의할 점 — 아래 버그 4개가 남아 있다 (전부 그대로 두고 표시만 했다)
	  [버그 1] input()의 scanf("%d", MAP[r][c]) 에 & 가 빠졌다.
	           값(-1)을 주소로 넘겨서 입력을 읽자마자 프로그램이 죽는다.
	           -> scanf("%d", &MAP[r][c]);
	  [버그 2] clean()이 이웃 칸을 0 밑으로 깎았을 때 이웃이 아니라 자기 칸을 0으로 만든다.
	           if (MAP[nr][nc] < 0) MAP[target.r][target.c] = 0;   -> MAP[nr][nc] = 0;
	           그래서 먼지가 음수(-10 등)로 남는다.
	           (가운데 5, 상하좌우 10 인 3 x 3 격자에서 청소 직후 이웃이 -10이 된다)
	  [버그 3] clean()이 자기 칸을 청소하지 않는다. 방향 고르기(getDirection)에서는
	           자기 칸 먼지까지 total에 넣었으므로, 청소도 자기 칸을 포함해야 맞다.
	           (버그 2 때문에 이웃이 음수가 되는 경우에만 우연히 자기 칸이 0이 된다)
	  [버그 4] getPosition()이 갈 곳을 못 찾았을 때 { sc, sc } 를 돌려준다. { sr, sc } 여야 한다.
	           먼지 칸에 닿을 수 없는 청소기가 엉뚱한 칸으로 순간이동한다.
*/
#include <stdio.h>

#define MAX (30+5)
#define WALL (-1)
#define INF (0x7fff0000)

int T;
int N, K, L; // 격자 크기, 로봇 청소기의 개수, 테스트 횟수

int MAP[MAX][MAX];
bool check[MAX][MAX]; //청소기 좌표

struct RC
{
	int r;
	int c;
};

RC cleaner[50+5];

RC queue[MAX * MAX];
int rcnt;

// 우,하,좌,상
int dr[] = { 0,1,0,-1 };
int dc[] = { 1,0,-1,0 };

void input()
{
	scanf("%d %d %d", &N, &K, &L);

	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= N + 1; c++)
			MAP[r][c] = WALL;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", MAP[r][c]);
			// [버그 1] & 가 빠졌다. scanf("%d", &MAP[r][c]) 여야 한다. 지금은 입력을 읽자마자 죽는다.

	for (int k = 1; k <= K; k++)
	{
		int r, c;

		scanf("%d %d", &r, &c);

		cleaner[k].r = r;
		cleaner[k].c = c;
	}
}

void printMap() // for debug
{
	for (int k = 1; k <= K; k++)
		printf("%d] %d, %d\n", k, cleaner[k].r, cleaner[k].c);
	putchar('\n');

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

RC getPosition(int index)	//BFS
{
	RC ret;
	int rp, wp;
	rp = wp = 0;
	int visit[MAX][MAX] = { 0 };

	int sr = cleaner[index].r;
	int sc = cleaner[index].c;

	if (MAP[sr][sc] > 0) return { sr,sc };
	// 지금 칸에 먼지가 있으면 움직이지 않는다.

	queue[wp].r = sr;
	queue[wp++].c = sc;

	visit[sr][sc] = 1;

	ret.r = ret.c = INF;
	int minDistance = INF;
	while (rp < wp)
	{
		RC out = queue[rp++];

		// 먼지를 찾은 경우 + 다른 청소기가 위치하지 않은 경우
		if (MAP[out.r][out.c] > 0 && check[out.r][out.c] == false)
		{
			if (visit[out.r][out.c] < minDistance)
			{
				minDistance = visit[out.r][out.c];
				ret = out;
			}
			else if (visit[out.r][out.c] == minDistance)
			{
				if (out.r < ret.r) ret = out;
				else if (out.r == ret.r)
				{
					if (out.c < ret.c)
						ret = out;
				}
			}
			continue;
		}

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			if (MAP[nr][nc] == WALL || check[nr][nc] == true) continue;
			if (visit[nr][nc] != 0) continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = visit[out.r][out.c] + 1;	
		}

	}

	if (ret.r == INF) return { sc,sc };
	// [버그 4] { sr, sc } 여야 한다. 갈 곳이 없으면 제자리에 있어야 한다.

	return ret;
}
int min(int a, int b)
{
	return a < b ? a : b;
}

int getDirection(int index)
{
	RC target = cleaner[index];

	int total = min(MAP[target.r][target.c], 20);
	// 자기 칸 + 상하좌우 다섯 칸에서 지울 수 있는 먼지(칸마다 최대 20)를 모두 더해 둔다.
	for (int i = 0; i < 4; i++)
	{
		int nr, nc;
		nr = target.r + dr[i];
		nc = target.c + dc[i];
		if (MAP[nr][nc] == WALL) continue;

		total += min(MAP[nr][nc], 20);
	}
	int maxDir = -1;
	int maxDust = -1;
	int changeDir[] = { 2,3,0,1 };

	for (int i = 0; i < 4; i++)
	{
		int reserve = changeDir[i];

		int nr, nc;

		nr = target.r + dr[reserve];
		nc = target.c + dc[reserve];

		int dust = total;
		// 방향 i를 바라보면 반대쪽 한 칸은 청소하지 않으므로 그만큼 뺀다.
		if (MAP[nr][nc] != WALL) dust -= min(MAP[nr][nc], 20);

		if (maxDust < dust)
			// "더 클 때만" 갱신 -> 같으면 우, 하, 좌, 상 순으로 앞선 방향이 남는다.
		{
			maxDust = dust;
			maxDir = i;
		}
	}

	return maxDir;
}

void clean(int index)
{
	int changeDir[] = { 2,3,0,1 };

	RC target = cleaner[index];
	int direction= getDirection(index);
	int reserve = changeDir[direction];

	for (int i = 0; i < 4; i++)
	{
		if (i == reserve) continue;

		int nr, nc;

		nr = target.r + dr[i];
		nc = target.c + dc[i];

		if (MAP[nr][nc] == WALL) continue;

		MAP[nr][nc] -= 20;
		if (MAP[nr][nc] < 0) MAP[target.r][target.c] = 0;
		// [버그 2] 이웃(MAP[nr][nc])을 0으로 만들어야 한다. 지금은 자기 칸을 0으로 만들어 이웃이 음수로 남는다.
		// [버그 3] 이 함수는 자기 칸(target)을 청소하지 않는다. 자기 칸도 최대 20 지워야 한다.
	}
}

void addDust()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == 0 || MAP[r][c] == WALL) continue;
			MAP[r][c] += 5;
		}
	}
}

void spreadDust()
{
	int tmpMAP[MAX][MAX] = { 0 };
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] != 0) continue;
			// 먼지가 0인 칸에만 생긴다. (물건 칸은 -1이라 여기서 같이 걸러진다)

			int sum = 0;
			for (int i = 0; i < 4; i++)
			{
				int nr, nc;
				nr = r + dr[i];
				nc = c + dc[i];

				if (MAP[nr][nc] == WALL) continue;

				sum += MAP[nr][nc];
			}
			tmpMAP[r][c] = sum / 10;
		}
	}

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] += tmpMAP[r][c];
}

int getDust()
{
	int sum = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == WALL)continue;
			sum += MAP[r][c];
		}
	}
	return sum;
}


void simulate()
{
	for (int l = 0; l < L; l++)
	{
		for (int r = 1; r <= N; r++)
			for (int c = 1; c <= N; c++)
				check[r][c] = false;

		// 0. 청소기 좌표 체크
		for (int k = 1; k <= K; k++)
			check[cleaner[k].r][cleaner[k].c] = true;

		// 1. 청소기 이동
		for (int k = 1; k <= K; k++)
		{
			RC rc = getPosition(k);

			check[cleaner[k].r][cleaner[k].c] = false;

			cleaner[k].r = rc.r;
			cleaner[k].c = rc.c;

			check[cleaner[k].r][cleaner[k].c] = true;

		}

		// 2. 청소
		for (int k = 1; k <= K; k++) clean(k);

		// 3. 먼지 축적
		addDust();

		// 4. 먼지 확산
		spreadDust();

		// 5. 결과 출력
		printf("%d\n", getDust());
	}

}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		simulate();
	}
	return 0;
}
