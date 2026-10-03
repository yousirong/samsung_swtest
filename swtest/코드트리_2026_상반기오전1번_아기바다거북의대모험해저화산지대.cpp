/*
	[코드트리] 2026 상반기 오전 1번 - 아기 바다거북의 대모험: 해저 화산 지대
	https://www.codetree.ai/training-field/frequent-problems   ("아기 바다거북" 검색)
	[확인 필요] 문제 개별 주소(slug)를 찾지 못해 기출 목록 주소를 달아 두었다.

	■ 문제 요약 (코드에서 읽어 낸 규칙이다. 세부 조건은 문제 본문과 대조가 필요하다)
	  N x N 바다(0은 바다, 1은 산호)에 아기 바다거북 M마리와 해저 화산 K개가 있다.
	  거북들은 오른쪽 아래 끝 (N-1, N-1)까지 가야 한다. 최대 100턴 동안 아래를 반복한다.

	    1) 거북 이동 : 1번 거북부터 차례로, 목적지 쪽 최단 경로로 한 칸 간다.
	       산호와 다른 거북이 있는 칸은 지나갈 수 없다. 길이 막혀 있으면 그 턴은 제자리다.
	       목적지에 닿으면 탈출한다(격자에서 빠진다).
	    2) 압력 증가 : 모든 화산의 압력이 10씩 오른다.
	    3) 분출 : 압력이 임계치 P 이상인 화산이 분출한다.
	       자기 칸에 P만큼, 상하좌우 네 방향으로 P/2, P/4, ... 의 열기를 퍼뜨린다.
	       (산호에 막히거나 값이 0이 되면 그 방향은 멈춘다)
	       받은 열기까지 더해 임계치를 넘는 화산은 연쇄로 분출한다. 더 없을 때까지 반복한다.
	    4) 화석화 : 이번 턴에 받은 열기가 20 이상인 칸의 거북은 화석이 된다.
	    5) 분출한 화산은 압력이 0으로 돌아간다.

	  거북마다 목적지에 도착한 턴 번호를 출력한다. 도착하지 못했거나 화석이 되었으면 -1.

	■ 풀이 방침
	  - 거북 이동은 BFS + before[] 역추적으로 "목적지까지 최단 경로의 첫 칸"을 구한다.
	    방향 순서를 우 -> 하 -> 좌 -> 상 으로 두어 같은 거리일 때의 우선순위를 맞춘다.
	  - 열기는 heat[][] 한 장에 이번 턴 동안 쌓는다. 화산은 압력(current)과 열기(heat)를 더해 판단한다.
	  - 연쇄 분출은 "이번 바퀴에 새로 터진 화산이 없을 때까지" while 로 반복한다.
	    volcano[k].check 로 이번 턴에 이미 터진 화산을 표시해 두 번 터지지 않게 한다.

	■ 상태 값 정리
	      turtle[m].check : 0 = 아직 진행 중, -1 = 끝(도착 또는 화석)
	      turtle[m].count : 지금까지 지난 턴 수. 화석이 되면 -1로 덮는다.
	  그래서 마지막 출력은
	      check == 0 (끝까지 도착 못 함)  -> -1
	      그 외                           -> count (도착 턴, 화석이면 -1)
	  로 한 줄에 정리된다.

	■ 주의할 점
	  - 이 문제는 좌표를 0부터 쓴다. 목적지는 (N-1, N-1)이다.
	  - 턴 수(count)는 움직이지 못한 턴에도 1씩 늘어난다.
	  - P[][]와 volcano[].check는 input()에서 초기화하지 않는다. 한 번만 실행하면 문제없지만
	    여러 번 돌리면 이전 값이 남는다.
	  - 분출 코드가 "처음 분출"과 "연쇄 분출"에 똑같이 두 번 적혀 있다. 하나를 고치면 둘 다 고쳐야 한다.
*/
#include <stdio.h>	

#define MAX (20+5)
#define CORAL (1)

int T;
int N, M, K;

int MAP[MAX][MAX];
int P[MAX][MAX]; // 분출 임계치 P
int current[MAX][MAX]; // 현재 압력

// 우하좌상
int dr[] = { 0,1,0,-1 };
int dc[] = { 1,0,-1,0 };

struct RC
{
	int r;
	int c;
};

RC queue[MAX * MAX];
int rp, wp;

struct RCC
{
	int r;
	int c;
	int check;
	int count; // 거북이 턴 번호
};

RCC volcano[1000 + 10];
RCC turtle[10 + 5];
int turtleMAP[MAX][MAX];

void input()
{
	scanf("%d %d %d", &N, &M, &K);

	for (int r = 0; r < N; r++)
		for (int c = 0; c < N; c++)
			scanf("%d", &MAP[r][c]);

	for (int r = 0; r < N; r++)
		for (int c = 0; c < N; c++)
			turtleMAP[r][c] = 0;

	for (int m = 1; m <= M; m++) // ID가 1번부터 시작
	{
		int r, c;

		scanf("%d %d", &r, &c);

		turtle[m].r = r;
		turtle[m].c = c;
		turtle[m].check = 0;
		turtle[m].count = 0;

		turtleMAP[r][c] = m;
	}

	for (int r = 0; r < N; r++)
		for (int c = 0; c < N; c++)
			current[r][c] = 0;

	for (int k = 0; k < K; k++)
	{
		int r, c, p;

		scanf("%d %d %d", &r, &c, &p);

		P[r][c] = p;
		volcano[k].r = r;
		volcano[k].c = c;
	}
}

void printMap(int map[MAX][MAX])
{
	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < N; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

RC getNextStep(int index)	// BFS
{
	RC ret;
	int rp, wp;
	int visit[MAX][MAX] = { 0 };
	RC before[MAX][MAX] = { 0 };

	rp = wp = 0;

	int sr = turtle[index].r;
	int sc = turtle[index].c;
	int er = N - 1;
	int ec = N - 1;

	queue[wp].r = sr;
	queue[wp++].c = sc;

	visit[sr][sc] = 1;

	before[sr][sc].r = -1;
	before[sr][sc].c = -1;

	while (rp < wp)
	{
		RC out = queue[rp++];

		if (er == out.r && ec == out.c)
		{
			int tr = er;
			int tc = ec;
			while (1)
			{
				int br, bc; // 이전 좌표

				br = before[tr][tc].r;
				bc = before[tr][tc].c;

				if (br == sr && bc == sc) break;

				tr = br;
				tc = bc;
			}
			ret.r = tr;
			ret.c = tc;

			return ret;
		}

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			if (nr<0 || nc<0 || nr>N - 1 || nc>N - 1) continue;
			if (visit[nr][nc] != 0 || MAP[nr][nc] != 0 || turtleMAP[nr][nc] != 0)continue;
			// 산호와 다른 거북이 있는 칸은 지나갈 수 없다.

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = visit[out.r][out.c] + 1;

			before[nr][nc] = out;
		}
	}

	ret.r = ret.c = -1;

	return ret; // for debug
}

void move()
{
	for (int m = 1; m <= M; m++)
	{
		if (turtle[m].check == -1) continue;

		turtle[m].count++;
		// 움직이지 못해도 한 턴이 지난다.

		RC next = getNextStep(m);

		if (next.r == -1) continue;

		int r, c, nr, nc;

		r = turtle[m].r;
		c = turtle[m].c;
		nr = next.r;
		nc = next.c;

		turtleMAP[r][c] = 0;

		turtle[m].r = nr;
		turtle[m].c = nc;

		turtleMAP[nr][nc] = m;

		if (nr == N - 1 && nc == N - 1)
		{
			turtle[m].check = -1;
			turtleMAP[nr][nc] = 0;
			// 도착한 거북은 격자에서 빠진다 (다른 거북의 길을 막지 않는다).
		}
	}
}

void increase()
{
	for (int k = 0; k < K; k++)
	{
		int r, c;

		r = volcano[k].r;
		c = volcano[k].c;

		current[r][c] += 10;
	}
}

void explode()
{
	int heat[MAX][MAX] = { 0 };

	// 열기 전파
	for (int k = 0; k < K; k++)
	{
		int r, c;

		r = volcano[k].r;
		c = volcano[k].c;

		if (current[r][c] >= P[r][c])
		{
			volcano[k].check = 1;
			heat[r][c] += P[r][c];

			for (int i = 0; i < 4; i++)
			{
				int nr, nc;

				nr = r + dr[i];
				nc = c + dc[i];

				int p = P[r][c] / 2;

				while (1)
				{
					if (nr<0 || nc<0 || nr>N - 1 || nc>N - 1) break;
					if (MAP[nr][nc] == CORAL || p == 0) break;

					heat[nr][nc] += p;
					// 멀어질수록 절반씩 줄어든다. 산호에 막히거나 0이 되면 멈춘다.

					p = p / 2;
					nr += dr[i];
					nc += dc[i];
				}
			}
		}
	}

	// 연쇄 반응
	while (1)
	{
		bool explodeCheck = false;

		for (int k = 0; k < K; k++)
		{
			if (volcano[k].check == 1) continue;

			int r, c;

			r = volcano[k].r;
			c = volcano[k].c;

			if (current[r][c] + heat[r][c] >= P[r][c])
				// 압력 + 이번 턴에 받은 열기가 임계치를 넘으면 연쇄로 터진다.
			{
				explodeCheck = true;

				volcano[k].check = 1;
				heat[r][c] += P[r][c];

				for (int i = 0; i < 4; i++)
				{
					int nr, nc;

					nr = r + dr[i];
					nc = c + dc[i];

					int p = P[r][c] / 2;

					while (1)
					{
						if (nr<0 || nc<0 || nr>N - 1 || nc>N - 1) break;
						if (MAP[nr][nc] == CORAL || p == 0)break;

						heat[nr][nc] += p;

						p = p / 2;
						nr += dr[i];
						nc += dc[i];
					}
				}
			}
		}
		if (explodeCheck == false) break;
		// 이번 바퀴에 새로 터진 화산이 없으면 연쇄가 끝난다.
	}

	// 바다거북의 위기 (화석화)
	for (int m = 1; m <= M; m++)
	{
		if (turtle[m].check == -1)continue;

		int r, c;

		r = turtle[m].r;
		c = turtle[m].c;

		if (heat[r][c] >= 20)
			// 열기 20 이상이면 화석이 된다. count를 -1로 덮어 출력 때 -1이 나오게 한다.
		{
			turtle[m].check = -1;
			turtle[m].count = -1;
		}
	}

}

void reset()
{
	for (int k = 0; k < K; k++)
	{
		int r, c;

		r = volcano[k].r;
		c = volcano[k].c;

		if (volcano[k].check == 1) current[r][c] = 0;

		volcano[k].check = 0;
	}
}

void simulate()
{
	for (int i = 0; i < 100; i++)
	{
		move();
		increase();
		explode();
		reset();
	}

	for (int m = 1; m <= M; m++)
	{
		//도착실패
		if (turtle[m].check == 0) printf("-1\n");
		else printf("%d\n", turtle[m].count);
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
