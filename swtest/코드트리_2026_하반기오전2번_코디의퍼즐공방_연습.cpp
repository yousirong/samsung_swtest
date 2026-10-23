#include <stdio.h>	

#define MAX (2000+5)
#define SHAPE (81)	
#define EMPTY (-1)

int T;
int N, Q;

int board[MAX][MAX];
long long holeCnt[SHAPE];
long long spareCnt[SHAPE];

// 위, 오른쪽, 아래, 왼쪽
int dr[] = { -1,0,1,0 };
int dc[] = { 0,1,0,-1 };

int makeShape(int u, int r, int d, int l)
{
	return u * 27 + r * 9 + d * 3 + 1;
}

int getSide(int shape, int dir)
{
	int div[] = { 27, 9,3,1 };
	return shape / div[dir] % 3;
}

// 시계 방향 90도 : a b c d -> d a b c
int rotate(int shape)
{
	return makeShape(getSide(shape, 3), getSide(shape, 0), getSide(shape, 1), getSide(shape, 2));
}

int getHole(int r, int c)
{
	if (r<1 || c<1 || r>N || c>N) return -1;
	if (board[r][c] != EMPTY) return -1;

	int side[4];
	for (int dir = 0; dir < 4; dir++)
	{
		int nr = r + dr[dir];
		int nc = c + dc[dir];

		if (nr<1 || nc<1 || nr>N || nc>N) side[dir] = 1;
		else if (board[nr][nc] == EMPTY) return -1;
		else side[dir] = 2 - getSide(board[nr][nc], (dir + 2) % 4);
	}

	return makeShape(side[0], side[1], side[2], side[3]);
}

void updateAround(int y, int x, int sign)
{
	int hole = getHole(y, x);
	if (hole != -1) holeCnt[hole] += sign;

	for (int dir = 0; dir < 4; dir++)
	{
		hole = getHole(y + dr[dir], x + dc[dir]);
		if (hole != -1) holeCnt[hole] += sign;
	}
}

//100 판 준비
void prepare(int n)
{
	N = n;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			board[r][c] = EMPTY;

	for (int s = 0; s < SHAPE; s++)
		holeCnt[s] = spareCnt[s] = 0;
}

// 200 배치
void setCell(int y, int x, int shape)
{
	updateAround(y, x, -1);
	board[y][x] = shape;
	updateAround(y, x, +1);
}

long long getScore()
{
	long long score = 0;

	for (int s = 0; s < SHAPE; s++)
	{
		if (holeCnt[s] == 0) continue;

		int rot[4];
		long long fit = 0;

		rot[0] = s;

		for (int i = 1; i < 4; i++) rot[i] = rotate(rot[i - 1]);

		for (int i = 0; i < 4; i++)
		{
			int dup = 0;
			for (int j = 0; j < i; j++)
				if (rot[j] == rot[i]) dup = 1;

			if (dup == 0) fit += spareCnt[rot[i]];
		}
		score+=holeCnt[s] * fit;
	}
	return score;
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		scanf("%d", &Q);

		for (int q = 0; q < Q; q++)
		{
			int cmd;
			scanf("%d", &cmd);

			if (cmd == 100)
			{
				int n;
				scanf("%d", &n);
				prepare(n);
			}
			else if (cmd == 200)
			{
				int y, x, u, r, d, l;
				scanf("%d %d %d %d %d %d", &y, &x, &u, &r, &d, &l);
				setCell(y, x, makeShape(u, r, d, l));
			}
			else if (cmd == 300)
			{
				int y, x;
				scanf("%d %d", &y, &x);
				setCell(y, x, EMPTY);
			}
			else if (cmd == 400)
			{
				int u, r, d, l;
				scanf("%d %d %d %d", &u, &r, &d, &l);
				spareCnt[makeShape(u, r, d, l)]++;
			}
			else if (cmd == 500)
			{
				printf("%lld\n", getScore());
			}
		}
	}
	return 0;
}