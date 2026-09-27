/*
	[코드트리] 2024 하반기 오전 1번 - 미지의 공간 탈출
	https://www.codetree.ai/training-field/frequent-problems/problems/escape-from-the-unknown-space

	■ 문제 요약
	  N x N 평지 위에 한 변이 M인 정육면체(시간의 벽)가 놓여 있다.
	  평지에는 빈칸(0), 장애물(1), 탈출구(4)가 있고 정육면체가 놓인 자리는 3으로 표시된다.
	  정육면체의 다섯 면(동, 서, 남, 북, 윗면)에도 각각 M x M 격자가 있고, 윗면에는 타임머신(2)이 있다.

	  타임머신에서 출발해 정육면체 면을 타고 내려와 평지로 나온 뒤 탈출구까지 가는
	  최소 이동 횟수를 구한다. 이동은 상하좌우 한 칸이고 장애물은 지날 수 없다.

	  여기에 "시간 이상 현상" F개가 있다. 각각 (행, 열, 방향, 속도 v)로 주어지며
	  평지의 그 칸에서 시작해 방향으로 퍼져 나간다. 시작 칸은 0초, 한 칸 갈 때마다 v초씩 늘어난다.
	  해당 칸은 그 시각이 되면 막히므로, 도착 시각이 그보다 빠를 때만 지나갈 수 있다.
	  탈출할 수 없으면 -1을 출력한다.

	■ 풀이 방침 : 면을 잇는 표(next)를 미리 만들고, 평면 BFS처럼 푼다
	  가장 어려운 부분은 "면의 경계를 넘어갈 때 어느 면의 어느 칸으로 가는가"다.
	  이것을 next[면][행][열][방향]에 미리 적어 두면(preprocess),
	  BFS 본체는 평면에서 하는 것과 똑같아진다.

	    - 옆면끼리(동 -> 북 -> 서 -> 남 -> 동)는 좌우로 이어진다.
	    - 윗면과 옆면은 방향에 따라 좌표가 뒤집히기도 한다 (M + 1 - i).
	    - 옆면의 아래 끝(M행)은 평지로 내려간다. 정육면체가 놓인 칸(3)의 바로 바깥 칸과 이어진다.
	      평지에서 정육면체로 들어가는 방향도 같은 표에 반대로 적어 둔다.

	  이렇게 여섯 장의 격자를 한 덩어리로 묶고 나면, 남는 것은
	  "장애물 피하기 + 시간 이상 현상보다 먼저 지나가기"뿐이다.

	■ 시간 이상 현상 퍼뜨리기 (makeTimeWall)
	  시작 칸에 1을 넣고(0초가 아니라 1로 두어 "값이 있음"과 구분한다),
	  방향으로 한 칸씩 가며 v씩 더한 값을 적는다.
	  벽이나 탈출구를 만나거나 평지를 벗어나면 멈춘다.
	  정육면체 칸(3)을 만나면 next 표를 따라 옆면으로 올라가며 계속 퍼진다.

	■ BFS에서 시간을 비교하는 방법
	  visit에는 "이동 횟수 + 1"이 들어간다(시작 칸이 1).
	  그래서 도착 시각은 visit - 1 이고, 마지막에 visit[BOTTOM][출구] - 1 을 답으로 낸다.
	  시간 이상 현상이 있는 칸은 아래 조건일 때만 지나간다.

	      visit[지금] + 1 < TIME_WALL[다음]

	  즉 "그 칸이 막히는 시각보다 먼저 도착"해야 한다.

	■ 주의할 점
	  - 전역 이름 next, end 는 <iterator> / <algorithm> 의 std::next, std::end 와 겹친다.
	    지금은 헤더를 stdio.h만 쓰고 using namespace std; 도 없어서 문제가 없다.
	    프로그래머스 제출용으로 옮길 때는 이름을 바꿔야 한다.
	  - 입력의 시간 이상 현상 좌표는 0-based로 들어오므로 +1 해서 저장한다.
	  - 평지 좌표계와 면 좌표계의 크기가 다르다(N vs M). 경계 검사에서 어느 쪽인지 늘 구분해야 한다.
*/
#include <stdio.h>

#define MAX (20+5)

#define EAST (0)
#define WEST (1)
#define SOUTH (2)
#define NORTH (3)
#define TOP (4)
#define BOTTOM (5)

#define EMPTY (0)
#define WALL (1)
#define TIME_MACHINE (2)
#define CUBE (3)
#define EXIT (4)

#define RIGHT (0)
#define LEFT (1)
#define DOWN (2)
#define UP  (3)

int T;

int N, M, F;
int MAP[6][MAX][MAX];
int visit[6][MAX][MAX];

int TIME_WALL[6][MAX][MAX];

struct RC
{
	int r;
	int c;
};

RC start, end;

struct PRC
{
	int p; // plane
	int r;
	int c;
};

PRC queue[MAX * MAX * 6];
PRC next[6][MAX][MAX][4];

struct TIME_INFO
{
	int p;
	int r;
	int c;
	int d;
	int v;
};

TIME_INFO timeInfo[10 + 3];

// 우, 좌, 하, 상
int dr[] = { 0,0,1,-1 };
int dc[] = { 1,-1,0,0 };

void input()
{
	scanf("%d %d %d", &N, &M, &F);

	//init
	for (int i = 0; i < 6; i++)
		for (int r = 0; r < MAX; r++)
			for (int c = 0; c < MAX; c++)
				MAP[i][r][c] = visit[i][r][c] = TIME_WALL[i][r][c] = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%d", &MAP[BOTTOM][r][c]);

			if (MAP[BOTTOM][r][c] == EXIT)
			{
				end.r = r;
				end.c = c;
			}
		}
	}
	

	for (int i = 0; i < 5; i++)
	{
		for (int r = 1; r <= M; r++)
		{
			for (int c = 1; c <= M; c++)
			{
				scanf("%d", &MAP[i][r][c]);

				if (MAP[TOP][r][c] == TIME_MACHINE)
				{
					start.r = r;
					start.c = c;
				}
			}
		}
	}

	// 시간 이상 현상
	for (int f = 0; f < F; f++)
	{
		int r, c, d, v;

		scanf("%d %d %d %d", &r, &c, &d, &v);

		timeInfo[f].p = BOTTOM;
		timeInfo[f].r = r + 1;
		// 입력이 0-based라 1-based로 맞춘다.
		timeInfo[f].c = c + 1;
		timeInfo[f].d = d;
		timeInfo[f].v = v;
	}
}

void printMap(int map[MAX][MAX], int size)	// for debug
{
	for (int r = 1; r <= size; r++)
	{
		for (int c = 1; c <= size; c++)
			printf("%2d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void printMapAll(int map[6][MAX][MAX]) // for debug
{
	printf("BOTTOM\n");
	printMap(map[BOTTOM], N);

	printf("EAST\n");
	printMap(map[EAST], N);

	printf("WEST\n");
	printMap(map[WEST], N);

	printf("SOUTH\n");
	printMap(map[SOUTH], N);

	printf("NORTH\n");
	printMap(map[NORTH], N);

	printf("TOP\n");
	printMap(map[TOP], N);
}

void preprocess()
{
	for (int i = 1; i <= M; i++)
	{
		// TOP (1, i)에서 ↑ 가면 NORTH (1, M + 1 - i)
		next[TOP][1][i][UP].p = NORTH;
		next[TOP][1][i][UP].r = 1;
		next[TOP][1][i][UP].c = M+1-i;

		// NORTH (1, i)에서 ↑ 가면 TOP (1, M + 1 - i)
		next[NORTH][1][i][UP].p = TOP;
		next[NORTH][1][i][UP].r = 1;
		next[NORTH][1][i][UP].c = M+1-i;

		// ------------------------------------- // 

		// TOP (M, i)에서 ↓ 가면 SOUTH (1, i)
		next[TOP][M][i][DOWN].p = SOUTH;
		next[TOP][M][i][DOWN].r = 1;
		next[TOP][M][i][DOWN].c = i;

		// SOUTH (1, i)에서 ↑ 가면 TOP (M, i)
		next[SOUTH][1][i][UP].p = TOP;
		next[SOUTH][1][i][UP].r = M;
		next[SOUTH][1][i][UP].c = i;

		// ------------------------------------- // 

		// TOP (i, M)에서 → 가면 EAST (1, M + 1 - i)
		next[TOP][i][M][RIGHT].p = EAST;
		next[TOP][i][M][RIGHT].r = 1;
		next[TOP][i][M][RIGHT].c = M + 1 - i;

		// EAST (1, i)에서 ↑ 가면 TOP (M + 1 - i, M)
		next[EAST][1][i][UP].p = TOP;
		next[EAST][1][i][UP].r = M + 1 - i;
		next[EAST][1][i][UP].c = M;

		// ------------------------------------- // 

		// TOP (i, 1)에서 ← 가면 WEST (1, i)
		next[TOP][i][1][LEFT].p = WEST;
		next[TOP][i][1][LEFT].r = 1;
		next[TOP][i][1][LEFT].c = i;

		// WEST (1, i)에서 ↑ 가면 TOP (i, 1)
		next[WEST][1][i][UP].p = TOP;
		next[WEST][1][i][UP].r = i;
		next[WEST][1][i][UP].c = 1;
	}

	// →
	for (int i = 1; i <= M; i++)
	{
		// SOUTH (i, M)에서 → 가면 EAST (i, 1)
		next[SOUTH][i][M][RIGHT].p = EAST;
		next[SOUTH][i][M][RIGHT].r = i;
		next[SOUTH][i][M][RIGHT].c = 1;

		// EAST (i, M)에서 → 가면 NORTH (i, 1)
		next[EAST][i][M][RIGHT].p = NORTH;
		next[EAST][i][M][RIGHT].r = i;
		next[EAST][i][M][RIGHT].c = 1;

		// NORTH (i, M)에서 → 가면 WEST (i, 1)
		next[NORTH][i][M][RIGHT].p = WEST;
		next[NORTH][i][M][RIGHT].r = i;
		next[NORTH][i][M][RIGHT].c = 1;

		// WEST (i, M)에서 → 가면 SOUTH (i, 1)
		next[WEST][i][M][RIGHT].p = SOUTH;
		next[WEST][i][M][RIGHT].r = i;
		next[WEST][i][M][RIGHT].c = 1;
	}

	// ←
	for (int i = 1; i <= M; i++)
	{
		// SOUTH (i, 1)에서 ← 가면 WEST (i, M)
		next[SOUTH][i][1][LEFT].p = WEST;
		next[SOUTH][i][1][LEFT].r = i;
		next[SOUTH][i][1][LEFT].c = M;

		// WEST (i, 1)에서 ← 가면 NORTH (i, M)
		next[WEST][i][1][LEFT].p = NORTH;
		next[WEST][i][1][LEFT].r = i;
		next[WEST][i][1][LEFT].c = M;

		// NORTH (i, 1)에서 ← 가면 EAST (i, M)
		next[NORTH][i][1][LEFT].p = EAST;
		next[NORTH][i][1][LEFT].r = i;
		next[NORTH][i][1][LEFT].c = M;

		// EAST (i, 1)에서 ← 가면 SOUTH (i, M)
		next[EAST][i][1][LEFT].p = SOUTH;
		next[EAST][i][1][LEFT].r = i;
		next[EAST][i][1][LEFT].c = M;
	}

	int index;

	// BOTTOM, WEST
	index = 1;
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[BOTTOM][r][c] == CUBE)
			{
				// BOTTOM → WEST
				next[BOTTOM][r][c - 1][RIGHT].p = WEST;
				next[BOTTOM][r][c - 1][RIGHT].r = M;
				next[BOTTOM][r][c - 1][RIGHT].c = index;

				// BOTTOM ← WEST = (↓)
				next[WEST][M][index][DOWN].p = BOTTOM;
				next[WEST][M][index][DOWN].r = r;
				next[WEST][M][index][DOWN].c = c - 1;

				index++;

				break;
			}
		}
	}

	// BOTTOM, EAST
	index = M;
	for (int r = 1; r <= N; r++)
	{
		for (int c = N; c >= 1; c--)
		{
			if (MAP[BOTTOM][r][c] == CUBE)
			{
				// EAST ← BOTTOM
				next[BOTTOM][r][c + 1][LEFT].p = EAST;
				next[BOTTOM][r][c + 1][LEFT].r = M;
				next[BOTTOM][r][c + 1][LEFT].c = index;

				// EAST → BOTTOM = (↓)
				next[EAST][M][index][DOWN].p = BOTTOM;
				next[EAST][M][index][DOWN].r = r;
				next[EAST][M][index][DOWN].c = c + 1;

				index--;

				break;
			}
		}
	}

	// BOTTOM, NORTH
	index = M;
	for (int c = 1; c <= N; c++)
	{
		for (int r = 1; r <= N; r++)
		{
			if (MAP[BOTTOM][r][c] == CUBE)
			{
				// BOTTOM 
				//   ↓
				// NORTH = ↓
				next[BOTTOM][r - 1][c][DOWN].p = NORTH;
				next[BOTTOM][r - 1][c][DOWN].r = M;
				next[BOTTOM][r - 1][c][DOWN].c = index;

				// BOTTOM 
				//   ↑
				// NORTH = ↓
				next[NORTH][M][index][DOWN].p = BOTTOM;
				next[NORTH][M][index][DOWN].r = r - 1;
				next[NORTH][M][index][DOWN].c = c;

				index--;
				break;
			}
		}
	}

	// BOTTOM, SOUTH
	index = 1;
	for (int c = 1; c <= N; c++)
	{
		for (int r = N; r >= 1; r--)
		{
			if (MAP[BOTTOM][r][c] == CUBE)
			{
				// SOUTH 
				//   ↑
				// BOTTOM = ↑
				next[BOTTOM][r + 1][c][UP].p = SOUTH;
				next[BOTTOM][r + 1][c][UP].r = M;
				next[BOTTOM][r + 1][c][UP].c = index;

				// SOUTH 
				//   ↓
				// BOTTOM = ↓
				next[SOUTH][M][index][DOWN].p = BOTTOM;
				next[SOUTH][M][index][DOWN].r = r + 1;
				next[SOUTH][M][index][DOWN].c = c;

				index++;
				break;
			}
		}
	}
}

void makeTimeWall()
{
	for (int f = 0; f < F; f++)
	{
		int p, r, c, d, v;

		p = timeInfo[f].p;
		r = timeInfo[f].r;
		c = timeInfo[f].c;
		d = timeInfo[f].d;
		v = timeInfo[f].v;

		TIME_WALL[p][r][c] = 1;
		// 시작 칸은 1로 둔다 (0은 "현상 없음"을 뜻하므로).

		while (1)
		{
			int np, nr, nc;

			np = p;
			nr = r + dr[d];
			nc = c + dc[d];

			if (p == BOTTOM && MAP[BOTTOM][nr][nc] == CUBE)
			// 평지에서 정육면체를 만나면 next 표를 따라 옆면으로 올라간다.
			{
				np = next[BOTTOM][r][c][d].p;
				nr = next[BOTTOM][r][c][d].r;
				nc = next[BOTTOM][r][c][d].c;
			}
			else if ((p != BOTTOM) && (nr<1 || nc<1 || nr>M || nc>M))
			{
				np = next[p][r][c][d].p;
				nr = next[p][r][c][d].r;
				nc = next[p][r][c][d].c;
			}

			if((np== BOTTOM)&& (nr<1|| nc<1|| nr>N || nc>N)) break;
			if (MAP[np][nr][nc] == WALL) break;
			if (nr == end.r && nc == end.c) break;
			// 탈출구에는 시간 이상 현상이 퍼지지 않는다.

			TIME_WALL[np][nr][nc] = TIME_WALL[p][r][c] + v;

			p = np;
			r = nr;
			c = nc;
		}
	}
}

int BFS(int r, int c)
{
	int rp, wp;
	rp = wp = 0;

	queue[wp].p = TOP;
	queue[wp].r = r;
	queue[wp++].c = c;

	visit[TOP][r][c] = 1;

	while (rp < wp)
	{
		PRC out = queue[rp++];

		if (out.p == BOTTOM && out.r == end.r && out.c == end.c)
			return visit[BOTTOM][out.r][out.c] - 1;
			// visit은 "이동 횟수 + 1"이라 1을 빼면 실제 이동 횟수다.

		for (int i = 0; i < 4; i++)
		{
			int np, nr, nc;

			np = out.p;
			nr = out.r +dr[i];
			nc = out.c +dc[i];

			if (out.p == BOTTOM && MAP[BOTTOM][nr][nc] == CUBE)
			{
				np = next[BOTTOM][out.r][out.c][i].p;
				nr = next[BOTTOM][out.r][out.c][i].r;
				nc = next[BOTTOM][out.r][out.c][i].c;
			}
			else if ((out.p != BOTTOM) && (nr < 1 || nc < 1 || nr > M || nc > M))
			{
				np = next[out.p][out.r][out.c][i].p;
				nr = next[out.p][out.r][out.c][i].r;
				nc = next[out.p][out.r][out.c][i].c;
			}

			if((np== BOTTOM) && (nr<1||nc<1|| nr>N|| nc>N)) continue;

			if (MAP[np][nr][nc] == WALL || visit[np][nr][nc] != 0)	continue;

			// 시간의 벽
			if(TIME_WALL[np][nr][nc]!=0
				&& visit[out.p][out.r][out.c] + 1 >= TIME_WALL[np][nr][nc]) continue;
			// 그 칸이 막히는 시각보다 먼저 도착해야 지나갈 수 있다.

			queue[wp].p = np;
			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[np][nr][nc] = visit[out.p][out.r][out.c] + 1;
		}
	}

	return -1;
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		preprocess();

		makeTimeWall();

		//printf("MAP\n"); printMapAll(MAP);

		//printf("TIME WALL\n"); printMapAll(TIME_WALL);


		printf("%d\n", BFS(start.r, start.c));

		//printf("visit\n"); printMapAll(visit);
	}

	return 0;
}
